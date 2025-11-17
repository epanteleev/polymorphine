#include <algorithm>
#include <ranges>

#include "AllocTemporalRegs.h"

#include "lir/x64/transform/regalloc/LinearScan.h"
#include "lir/x64/instruction/LIRAdjustStack.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsJoinEval.h"

static std::pair<aasm::GPRegSet, aasm::XmmRegSet> collect_used_argument_regs(const std::span<LIRVal const>& args) {
    aasm::GPRegSet used_gp_argument_regs;
    aasm::XmmRegSet used_xmm_argument_regs;
    for (const auto& arg: args) {
        const auto reg = arg.assigned_reg().to_reg();
        if (!reg.has_value()) {
            continue;
        }
        if (const auto gp_reg = reg->as_gp_reg(); gp_reg.has_value()) {
            used_gp_argument_regs.emplace(gp_reg.value());
            continue;
        }

        if (const auto xmm_reg = reg->as_xmm_reg(); xmm_reg.has_value()) {
            used_xmm_argument_regs.emplace(xmm_reg.value());
        }
    }

    return std::make_pair(used_gp_argument_regs, used_xmm_argument_regs);
}

void LinearScan::run() {
    allocate_fixed_registers();
    instruction_ordering();
    setup_unhandled_intervals();
    do_register_allocation();
    finalize_prologue_epilogue();
}

LinearScan LinearScan::create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data, aasm::SymbolTable &symbol_tab, const call_conv::CallConvProvider *call_conv)  {
    const auto intervals = cache->analyze<LiveIntervalsEval>(data);
    const auto joins = cache->analyze<LiveIntervalsJoinEval>(data);
    const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
    const auto [gp_regs, xmm_regs] = collect_used_argument_regs(data->args());

    auto vreg_selection = details::VRegSelection::create(call_conv, gp_regs, xmm_regs);
    return LinearScan(*data, std::move(vreg_selection), *intervals, *joins, *preorder, symbol_tab, call_conv);
}

void LinearScan::allocate_fixed_registers() {
    for (const auto& group: m_groups) {
        const auto fixed_reg = group.fixed_register();
        if (!fixed_reg.has_value()) {
            continue;
        }

        for (const auto& lir: group.m_values) {
            allocate_register(lir, fixed_reg.value());
        }
    }
}

void LinearScan::setup_unhandled_intervals() {
    m_unhandled_intervals.reserve(m_intervals.intervals().size());
    m_active_intervals.reserve(m_intervals.intervals().size());

    for (auto& [lir_val, interval]: m_intervals.intervals()) {
        if (lir_val.arg().has_value()) {
            m_active_intervals.emplace_back(&interval, lir_val);
        } else {
            m_unhandled_intervals.emplace_back(&interval, lir_val);
        }
    }

    const auto pred = [](const IntervalEntry& lhs, const IntervalEntry& rhs) {
        return lhs.interval->start() > rhs.interval->start();
    };

    std::ranges::sort(m_unhandled_intervals, pred);
}

void LinearScan::do_register_allocation()  {
    std::int64_t range_begin{};
    while (!m_unhandled_intervals.empty()) {
        auto [unhandled_interval, lir_val] = m_unhandled_intervals.back();
        m_unhandled_intervals.pop_back();

        if (lir_val.isa(gen_v())) {
            do_stack_alloc(lir_val);
            continue;
        }
        for (std::int64_t i = range_begin; i < unhandled_interval->start()-1; ++i) {
            allocate_temporal_register(m_instruction_ordering[i]);
        }
        range_begin = unhandled_interval->start();

        erase_active(*unhandled_interval);
        erase_unactive(*unhandled_interval);
        activate_unhandled_fixed_regs(*unhandled_interval);

        select_virtual_reg(lir_val, unhandled_interval->hint());
        m_active_intervals.emplace_back(unhandled_interval, lir_val);

        // Allocate temporals for current instruction.
        allocate_temporal_register(m_instruction_ordering[range_begin-1]);
    }

    // Process the remaining instructions.
    for (std::int64_t i = range_begin; i < static_cast<std::int64_t>(m_instruction_ordering.size()); ++i) {
        allocate_temporal_register(m_instruction_ordering[i]);
    }
}

void LinearScan::finalize_prologue_epilogue() const {
    const auto prologue = m_obj_func_data.prologue();
    const auto epilogue = m_obj_func_data.epilogue();
    for (const auto reg: m_used_callee_saved_regs) {
        epilogue->add_reg(reg);
        prologue->add_reg(reg);
    }
    epilogue->increase_local_area_size(m_reg_set.local_area_size());
    prologue->increase_local_area_size(m_reg_set.local_area_size());
}

void LinearScan::erase_active(const LiveInterval& unhandled_interval) {
    const auto active_eraser = [&](const IntervalEntry& entry) {
        const auto real_interval = get_real_interval(entry);
        const auto reg = entry.lir_val.assigned_reg();
        if (real_interval->start() > unhandled_interval.finish()) {
            m_reg_set.try_push(reg);
            return true;
        }

        if (real_interval->intersects(unhandled_interval)) {
            return false;
        }
        m_inactive_intervals.emplace_back(entry);
        m_reg_set.try_push(reg);
        return true;
    };

    remove_if_fast(m_active_intervals, active_eraser);
}

void LinearScan::erase_unactive(const LiveInterval &unhandled_interval) {
    const auto unactive_eraser = [&](const IntervalEntry& entry) {
        const auto real_interval = get_real_interval(entry);
        if (real_interval->start() > unhandled_interval.finish()) {
            return true;
        }

        if (!real_interval->intersects(unhandled_interval)) {
            return false;
        }
        // This interval is still active, we need to keep it.
        m_active_intervals.emplace_back(entry);
        m_reg_set.try_remove(entry.lir_val.assigned_reg());
        return true;
    };

    remove_if_fast(m_inactive_intervals, unactive_eraser);
}

void LinearScan::activate_unhandled_fixed_regs(const LiveInterval &unhandled_interval) {
    for (const auto& unhandled: std::ranges::reverse_view(m_unhandled_intervals)) {
        const auto group = m_groups.try_get_group(unhandled.lir_val);
        if (!group.has_value()) {
            // No group for this vreg, we can skip it.
            continue;
        }

        const auto real_interval = &group.value()->m_interval;
        if (real_interval->start() > unhandled_interval.finish()) {
            // No need to check further, the intervals are sorted.
            break;
        }

        const auto fixed_reg_groups = group.value()->fixed_register();
        if (!fixed_reg_groups.has_value()) {
            // This group does not have a fixed register, we can skip it too.
            continue;
        }
        const auto fixed_gp_reg = fixed_reg_groups.value().as_gp_reg();
        if (!fixed_gp_reg.has_value()) {
            continue;
        }

        if (!real_interval->intersects(unhandled_interval)) {
            // This interval does not intersect with the unhandled interval, skip it.
            continue;
        }

        m_active_intervals.emplace_back(unhandled);
        m_reg_set.remove(fixed_gp_reg.value());
    }
}

const LiveInterval *LinearScan::get_real_interval(const IntervalEntry &entry) const {
    if (const auto it = m_groups.try_get_group(entry.lir_val); it.has_value()) {
        return &it.value()->interval();
    }

    return entry.interval;
}

void LinearScan::select_virtual_reg(const LIRVal &lir_val, const IntervalHint hint) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }

    if (const auto group = m_groups.try_get_group(lir_val); group.has_value()) {
        assertion(!group.value()->fixed_register().has_value(), "Group with fixed register should not be allocated here");
        const auto reg = m_reg_set.top(group.value()->hint(), lir_val.type());
        for (const auto& group_vreg: group.value()->m_values) {
            allocate_register(group_vreg, reg);
        }
        return;
    }

    const auto reg = m_reg_set.top(hint, lir_val.type());
    allocate_register(lir_val, reg);
}

void LinearScan::allocate_register(const LIRVal &lir_val, const GPVReg &reg) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }

    if (const auto gp_reg= reg.as_gp_reg(); gp_reg.has_value()) {
        allocate_register(lir_val, gp_reg.value());
        return;
    }

    lir_val.assign_reg(reg);
}

void LinearScan::allocate_register(const LIRVal &lir_val, const aasm::Reg &reg) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }

    const auto vis = [&]<typename T>(const T& r) {
        allocate_register(lir_val, r);
    };

    reg.visit(vis);
}

void LinearScan::allocate_register(const LIRVal &lir_val, const aasm::GPReg reg) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }
    lir_val.assign_reg(reg);

    if (!std::ranges::contains(m_call_conv->GP_CALLEE_SAVE_REGISTERS(), reg)) {
        // This is a caller-save register, we can skip it.
        return;
    }

    if (std::ranges::contains(m_used_callee_saved_regs, reg)) {
        return; // Already added
    }

    m_used_callee_saved_regs.emplace(reg);
}

void LinearScan::allocate_register(const LIRVal &lir_val, const aasm::XmmReg reg) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }

    lir_val.assign_reg(reg);
}

void LinearScan::allocate_temporal_register(LIRInstructionBase *inst) noexcept {
    if (!inst->temporal_regs().empty()) {
        return;
    }
    const auto [gp_num, xmm_num] = details::AllocTemporalRegs::allocate(m_symbol_tab, inst);
    std::optional<aasm::XmmReg> xmm_reg;
    switch (xmm_num) {
        case 0: break;
        case 1: {
            const auto reg = m_reg_set.top_xmm(IntervalHint::NOTHING);
            m_reg_set.push(reg);
            xmm_reg = reg;
            break;
        }
        default: std::unreachable();
    }

    switch (gp_num) {
        case 0: {
            inst->init_temporal_regs(TemporalRegs(xmm_reg));
            break;
        }
        case 1: {
            const auto reg = m_reg_set.top(IntervalHint::NOTHING, LIRValType::GP);
            m_reg_set.push(reg);
            inst->init_temporal_regs(TemporalRegs(reg.as_gp_reg().value(), xmm_reg));
            break;
        }
        case 2: {
            const auto reg1 = m_reg_set.top(IntervalHint::NOTHING, LIRValType::GP);
            const auto reg2 = m_reg_set.top(IntervalHint::NOTHING, LIRValType::GP);
            m_reg_set.push(reg2);
            m_reg_set.push(reg1);
            inst->init_temporal_regs(TemporalRegs(reg1.as_gp_reg().value(), reg2.as_gp_reg().value(), xmm_reg));
            break;
        }
        default: die("Unexpected number of temporal registers allocated: {}", gp_num);
    }
}

void LinearScan::do_stack_alloc(const LIRVal &lir_val) {
    if (!lir_val.assigned_reg().empty()) {
        return;
    }

    lir_val.assign_reg(m_reg_set.stack_alloc(lir_val.size(), lir_val.alignment()));
}

void LinearScan::instruction_ordering() {
    const auto fn = [](const std::size_t acc, const LIRBlock* bb) { return acc + bb->size(); };
    const auto size = std::ranges::fold_left(m_preorder, 0UL, fn);

    m_instruction_ordering.reserve(size);
    for (const auto bb: m_preorder) {
        for (auto& inst: bb->instructions()) {
            m_instruction_ordering.push_back(&inst);
        }
    }
}
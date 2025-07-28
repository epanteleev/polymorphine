#pragma once

#include "FixedRegisters.h"
#include "FixedRegistersEval.h"
#include "RegisterAllocation.h"
#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "utility/Align.h"

class LinearScan final {
public:
    using result_type = RegisterAllocation;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScan(const LIRFuncData &obj_func_data, const FixedRegisters& fixed_registers, const LiveIntervals& intervals) noexcept:
        m_obj_func_data(obj_func_data),
        m_fixed_registers(fixed_registers),
        m_intervals(intervals) {}

public:
    void run() {
        allocate_fixed_registers();
        setup_unhandled_intervals();
        do_register_allocation();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<RegisterAllocation>(std::move(m_reg_allocation), m_local_area_size);
    }

    static LinearScan create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto fixed_registers = cache->analyze<FixedRegistersEval>(data);
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);

        return {*data, *fixed_registers, *intervals};
    }

private:
    void allocate_fixed_registers() {
        for (const auto& [lir, rax_reg]: m_fixed_registers) {
            m_reg_allocation.emplace(lir, rax_reg);
        }
    }

    void setup_unhandled_intervals() {
        m_unhandled_intervals.reserve(m_intervals.intervals().size());
        for (auto& [lir_val, interval]: m_intervals.intervals()) {
            if (lir_val.arg().has_value()) {
                m_active_intervals.emplace_back(&interval, lir_val);
            } else {
                m_unhandled_intervals.emplace_back(&interval, lir_val);
            }
        }
    }

    void do_register_allocation() {
        auto reg_set = RegSet::create(std::array{aasm::rdi});
        for (auto& [unhandled_interval, vreg]: m_unhandled_intervals) {
            if (vreg.isa(gen())) {
                const auto offset = align_up(m_local_area_size, vreg.size()) + vreg.size();
                m_reg_allocation.try_emplace(vreg, aasm::Address(aasm::rbp, 0, -offset));
                m_local_area_size = offset;
                continue;
            }

            const auto active_eraser = [&](const IntervalEntry& entry) {
                if (entry.m_interval->end() <= unhandled_interval->begin()) {
                    // This interval is no longer active, we can remove it.
                    return true;
                }

                if (!entry.m_interval->intersects(*unhandled_interval)) {
                    m_inactive_intervals.emplace_back(entry);
                    const auto reg = m_reg_allocation.at(entry.m_vreg);
                    if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                        reg_set.push(reg_opt.value());
                    }

                    return true;
                }

                return false;
            };

            std::erase_if(m_active_intervals, active_eraser);

            const auto unactive_eraser = [&](const IntervalEntry& entry) {
                if (entry.m_interval->end() <= unhandled_interval->begin()) {
                    // This interval is no longer active, we can remove it.
                    return true;
                }
                if (entry.m_interval->intersects(*unhandled_interval)) {
                    // This interval is still active, we need to keep it.
                    m_active_intervals.emplace_back(entry);
                    const auto reg = m_reg_allocation.at(entry.m_vreg);
                    if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                        reg_set.remove(reg_opt.value());
                    }

                    return true;
                }

                return false;
            };

            std::erase_if(m_inactive_intervals, unactive_eraser);

            for (const auto& unhandled: m_unhandled_intervals) {
                if (!m_fixed_registers.contains(unhandled.m_vreg)) {
                    continue;
                }

                if (!unhandled.m_interval->intersects(*unhandled_interval)) {
                    continue;
                }

                m_active_intervals.emplace_back(unhandled);
                const auto reg = m_reg_allocation.at(unhandled.m_vreg);
                if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                    reg_set.remove(reg_opt.value());
                }
            }

            const auto pair = m_reg_allocation.try_emplace(vreg, reg_set.top());
            if (pair.second) {
                // Successfully allocated a register for this interval.
                reg_set.pop();
            }
            m_active_intervals.emplace_back(unhandled_interval, vreg);
        }
    }

    struct IntervalEntry final {
        IntervalEntry(const LiveInterval* interval, const LIRVal vreg) noexcept:
            m_interval(interval),
            m_vreg(vreg) {}

        const LiveInterval* m_interval;
        LIRVal m_vreg;
    };

    class RegSet final {
    public:
        using stack = std::vector<aasm::GPReg>;

        explicit RegSet(stack&& free_regs) noexcept:
            m_free_regs(std::move(free_regs)) {}

        template<std::ranges::range Range>
        static RegSet create(Range&& arg_regs) {
            stack regs{};
            for (const auto& reg: call_conv::GP_CALLER_SAVE_REGISTERS) {
                if (std::ranges::contains(arg_regs, reg)) {
                    continue;
                }

                regs.push_back(reg);
            }

            return RegSet(std::move(regs));
        }

        [[nodiscard]]
        aasm::GPReg top() const noexcept {
            return m_free_regs.back();
        }

        void pop() noexcept {
            m_free_regs.pop_back();
        }

        void push(const aasm::GPReg reg) noexcept {
            m_free_regs.push_back(reg);
        }

        void remove(const aasm::GPReg reg) noexcept {
            if (const auto it = std::ranges::find(m_free_regs, reg); it != m_free_regs.end()) {
                m_free_regs.erase(it);
            }
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return m_free_regs.empty();
        }

    private:
        stack m_free_regs{};
    };

    const LIRFuncData& m_obj_func_data;
    const FixedRegisters& m_fixed_registers;
    const LiveIntervals& m_intervals;

    LIRValMap<GPVReg> m_reg_allocation{};
    // Size of the 'gen' values in the local area.
    std::int32_t m_local_area_size{0};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};
};

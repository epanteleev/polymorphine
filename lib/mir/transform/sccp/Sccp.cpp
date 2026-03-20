#include "Sccp.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <concepts>
#include <optional>

#include "mir/instruction/Binary.h"
#include "mir/instruction/Icmp.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Select.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/ValueInstruction.h"
#include "mir/types/FloatingPointType.h"
#include "mir/types/IntegerType.h"
#include "mir/value/VConstant.h"

namespace {
    enum class LatticeKind : std::uint8_t {
        Unknown,
        Constant,
        Overdefined
    };

    class LatticeValue final {
        explicit constexpr LatticeValue(const LatticeKind _kind, const Value& _value) noexcept:
            kind(_kind),
            value(_value) {}

    public:
        consteval LatticeValue() noexcept:
            kind(LatticeKind::Unknown),
            value(Value::undefined()) {}

        [[nodiscard]]
        static consteval LatticeValue unknown() noexcept {
            return LatticeValue(LatticeKind::Unknown, Value::undefined());
        }

        [[nodiscard]]
        static consteval LatticeValue overdefined() noexcept {
            return LatticeValue(LatticeKind::Overdefined, Value::undefined());
        }

        [[nodiscard]]
        static LatticeValue constant(const Value& val) noexcept {
            return LatticeValue(LatticeKind::Constant, val);
        }

        LatticeKind kind{};
        Value value;
    };

    [[nodiscard]]
    std::optional<bool> as_condition_value(const Value& value) noexcept {
        const auto fun = []<typename T>(const T& raw) -> std::optional<bool> {
            if constexpr (std::same_as<T, bool>) {
                return raw;
            } else {
                return std::nullopt;
            }
        };
        return value.visit(fun);
    }

    [[nodiscard]]
    Value make_int_value(const IntegerType* type, const std::int64_t value) noexcept {
        if (SignedIntegerType::cast(type) != nullptr) {
            switch (type->size_of()) {
                case 1: return Value::i8(static_cast<std::int8_t>(value));
                case 2: return Value::i16(static_cast<std::int16_t>(value));
                case 4: return Value::i32(static_cast<std::int32_t>(value));
                case 8: return Value::i64(value);
                default: std::unreachable();
            }
        }

        assertion(UnsignedIntegerType::cast(type) != nullptr, "expected unsigned integer type");
        switch (type->size_of()) {
            case 1: return Value::u8(static_cast<std::uint8_t>(value));
            case 2: return Value::u16(static_cast<std::uint16_t>(value));
            case 4: return Value::u32(static_cast<std::uint32_t>(value));
            case 8: return Value::u64(static_cast<std::uint64_t>(value));
            default: std::unreachable();
        }
    }

    [[nodiscard]]
    Value make_fp_value(const FloatingPointType* type, const double value) noexcept {
        switch (type->size_of()) {
            case 4: return Value::f32(static_cast<float>(value));
            case 8: return Value::f64(value);
            default: std::unreachable();
        }
    }

    [[nodiscard]]
    bool is_removable_dead_instruction(const Instruction* inst) noexcept {
        const auto* value_inst = ValueInstruction::cast(inst);
        if (value_inst == nullptr) {
            return false;
        }
        if (dynamic_cast<const TerminateValueInstruction*>(inst) != nullptr) {
            return false;
        }

        return true;
    }

    class SCCPEval final {
    public:
        explicit SCCPEval(FunctionData& fn) noexcept:
            m_fn(fn) {}

        void run() {
            m_executable_blocks.emplace(m_fn.first());

            bool changed = false;
            do {
                changed = false;
                for (auto& bb: m_fn.basic_blocks()) {
                    if (!m_executable_blocks.contains(&bb)) {
                        continue;
                    }
                    changed |= process_block(bb);
                }
            } while (changed);

            rewrite_constants();
            simplify_branches();
            remove_dead_blocks();
            remove_dead_instructions();
        }

    private:
        [[nodiscard]]
        LatticeValue lattice_of_operand(const Value& operand) const noexcept {
            const auto cst = VConstant::try_from(operand);
            if (cst.has_value()) {
                return LatticeValue::constant(operand);
            }

            if (!operand.is<ValueInstruction*>()) {
                return LatticeValue::overdefined();
            }

            const auto* def = operand.get<ValueInstruction*>();
            if (const auto it = m_states.find(def); it != m_states.end()) {
                return it->second;
            }

            return LatticeValue::unknown();
        }

        [[nodiscard]]
        bool merge_state(const ValueInstruction* inst, const LatticeValue &incoming) {
            auto& current = m_states[inst];
            if (current.kind == LatticeKind::Overdefined || incoming.kind == LatticeKind::Unknown) {
                return false;
            }
            if (incoming.kind == LatticeKind::Overdefined) {
                if (current.kind == LatticeKind::Overdefined) {
                    return false;
                }
                current = incoming;
                return true;
            }

            // incoming is constant.
            if (current.kind == LatticeKind::Unknown) {
                current = incoming;
                return true;
            }
            if (current.kind == LatticeKind::Constant) {
                if (current.value == incoming.value) {
                    return false;
                }
                current = LatticeValue::overdefined();
                return true;
            }

            return false;
        }

        [[nodiscard]]
        LatticeValue evaluate_phi(const Phi* phi) const noexcept {
            std::optional<Value> acc;
            for (std::size_t i{}; i < phi->incoming().size(); ++i) {
                if (!m_executable_blocks.contains(phi->incoming()[i])) {
                    continue;
                }

                const auto [kind, value] = lattice_of_operand(phi->operands()[i]);
                if (kind == LatticeKind::Overdefined) {
                    return LatticeValue::overdefined();
                }
                if (kind == LatticeKind::Unknown) {
                    return LatticeValue::unknown();
                }

                if (!acc.has_value()) {
                    acc = value;
                    continue;
                }

                if (acc.value() != value) {
                    return LatticeValue::overdefined();
                }
            }

            if (!acc.has_value()) {
                return LatticeValue::unknown();
            }

            return LatticeValue::constant(acc.value());
        }

        [[nodiscard]]
        LatticeValue evaluate_binary(const Binary* bin) const noexcept {
            const auto lhs = lattice_of_operand(bin->lhs());
            const auto rhs = lattice_of_operand(bin->rhs());
            if (lhs.kind == LatticeKind::Overdefined || rhs.kind == LatticeKind::Overdefined) {
                return LatticeValue::overdefined();
            }
            if (lhs.kind != LatticeKind::Constant || rhs.kind != LatticeKind::Constant) {
                return LatticeValue::unknown();
            }

            if (const auto* int_type = IntegerType::cast(bin->type())) {
                const auto lhs_i = lhs.value.get<std::int64_t>();
                const auto rhs_i = rhs.value.get<std::int64_t>();
                switch (bin->op()) {
                    case BinaryOp::Add: {
                        const auto cst = make_int_value(int_type, lhs_i + rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::Subtract: {
                        const auto cst = make_int_value(int_type, lhs_i - rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::Multiply: {
                        const auto cst = make_int_value(int_type, lhs_i * rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::Divide: {
                        if (rhs_i == 0) {
                            return LatticeValue::overdefined();
                        }

                        const auto cst = make_int_value(int_type, lhs_i / rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::BitwiseAnd: {
                        const auto cst = make_int_value(int_type, lhs_i & rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::BitwiseOr: {
                        const auto cst = make_int_value(int_type, lhs_i | rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::BitwiseXor: {
                        const auto cst = make_int_value(int_type, lhs_i ^ rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::ShiftLeft: {
                        if (rhs_i < 0 || rhs_i >= 64) {
                            return LatticeValue::overdefined();
                        }

                        const auto cst = make_int_value(int_type, lhs_i << rhs_i);
                        return LatticeValue::constant(cst);
                    }
                    case BinaryOp::ShiftRight: {
                        if (rhs_i < 0 || rhs_i >= 64) {
                            return LatticeValue::overdefined();
                        }
                        if (SignedIntegerType::cast(int_type) != nullptr) {
                            const auto cst = make_int_value(int_type, lhs_i >> rhs_i);
                            return LatticeValue::constant(cst);
                        }
                        const auto lhs_u = static_cast<std::uint64_t>(lhs_i);
                        const auto res_u = lhs_u >> rhs_i;
                        const auto cst = make_int_value(int_type, static_cast<std::int64_t>(res_u));
                        return LatticeValue::constant(cst);
                    }
                    default: std::unreachable();
                }
            }

            if (const auto* fp_type = FloatingPointType::cast(bin->type())) {
                const auto lhs_fp = lhs.value.get<double>();
                const auto rhs_fp = rhs.value.get<double>();
                switch (bin->op()) {
                    case BinaryOp::Add: {
                        const auto fp = make_fp_value(fp_type, lhs_fp + rhs_fp);
                        return LatticeValue::constant(fp);
                    }
                    case BinaryOp::Subtract: {
                        const auto fp = make_fp_value(fp_type, lhs_fp - rhs_fp);
                        return LatticeValue::constant(fp);
                    }
                    case BinaryOp::Multiply: {
                        const auto fp = make_fp_value(fp_type, lhs_fp * rhs_fp);
                        return LatticeValue::constant(fp);
                    }
                    case BinaryOp::Divide: {
                        const auto fp = make_fp_value(fp_type, lhs_fp / rhs_fp);
                        return LatticeValue::constant(fp);
                    }
                    default: return LatticeValue::overdefined();
                }
            }

            return LatticeValue::overdefined();
        }

        [[nodiscard]]
        LatticeValue evaluate_select(const Select* select) const noexcept {
            const auto cond = lattice_of_operand(select->condition());
            if (cond.kind == LatticeKind::Unknown) {
                return LatticeValue::unknown();
            }

            if (cond.kind == LatticeKind::Constant) {
                const auto cond_value = as_condition_value(cond.value);
                if (!cond_value.has_value()) {
                    return LatticeValue::overdefined();
                }
                const auto branch = cond_value.value() ? select->on_true() : select->on_false();
                return lattice_of_operand(branch);
            }

            const auto on_true = lattice_of_operand(select->on_true());
            const auto on_false = lattice_of_operand(select->on_false());

            if (on_true.kind == LatticeKind::Constant && on_false.kind == LatticeKind::Constant) {
                if (on_true.value == on_false.value) {
                    return on_true;
                }

                return LatticeValue::overdefined();
            }
            if (on_true.kind == LatticeKind::Overdefined || on_false.kind == LatticeKind::Overdefined) {
                return LatticeValue::overdefined();
            }

            return LatticeValue::unknown();
        }

        bool process_block(const BasicBlock& bb) {
            bool changed = false;
            for (const auto& inst: bb.instructions()) {
                if (const auto* phi = Phi::cast(&inst)) {
                    changed |= merge_state(phi, evaluate_phi(phi));
                    continue;
                }

                if (const auto* bin = dynamic_cast<const Binary*>(&inst)) {
                    changed |= merge_state(bin, evaluate_binary(bin));
                } else if (const auto* sel = dynamic_cast<const Select*>(&inst)) {
                    changed |= merge_state(sel, evaluate_select(sel));
                }

                if (const auto* cond = dynamic_cast<const CondBranch*>(&inst)) {
                    changed |= process_cond_branch(cond);
                    continue;
                }

                if (const auto* icmp = dynamic_cast<const IcmpInstruction*>(&inst)) {
                    changed |= process_icmp(icmp);
                    continue;
                }

                if (const auto term = Terminator::from(&inst); term.has_value()) {
                    for (const auto* succ: term->targets()) {
                        changed |= m_executable_blocks.emplace(succ).second;
                    }
                }
            }
            return changed;
        }

        bool process_icmp(const IcmpInstruction* icmp) {
            const auto lhs = lattice_of_operand(icmp->lhs());
            const auto rhs = lattice_of_operand(icmp->rhs());
            if (lhs.kind == LatticeKind::Overdefined || rhs.kind == LatticeKind::Overdefined) {
                return merge_state(icmp, LatticeValue::overdefined());
            }
            if (lhs.kind != LatticeKind::Constant || rhs.kind != LatticeKind::Constant) {
                return false;
            }
            if (!lhs.value.is<std::int64_t>() || !rhs.value.is<std::int64_t>()) {
                return merge_state(icmp, LatticeValue::overdefined());
            }

            const auto lhs_i = lhs.value.get<std::int64_t>();
            const auto rhs_i = rhs.value.get<std::int64_t>();
            const bool is_signed = SignedIntegerType::cast(icmp->lhs().type()) != nullptr;

            bool pred_result = false;
            switch (icmp->predicate()) {
                case IcmpPredicate::Eq: pred_result = lhs_i == rhs_i; break;
                case IcmpPredicate::Ne: pred_result = lhs_i != rhs_i; break;
                case IcmpPredicate::Lt: {
                    if (is_signed) {
                        pred_result = lhs_i < rhs_i;
                    } else {
                        pred_result = static_cast<std::uint64_t>(lhs_i) < static_cast<std::uint64_t>(rhs_i);
                    }
                    break;
                }
                case IcmpPredicate::Le: {
                    if (is_signed) {
                        pred_result = lhs_i <= rhs_i;
                    } else {
                        pred_result = static_cast<std::uint64_t>(lhs_i) <= static_cast<std::uint64_t>(rhs_i);
                    }
                    break;
                }
                case IcmpPredicate::Gt: {
                    if (is_signed) {
                        pred_result = lhs_i > rhs_i;
                    } else {
                        pred_result = static_cast<std::uint64_t>(lhs_i) > static_cast<std::uint64_t>(rhs_i);
                    }
                    break;
                }
                case IcmpPredicate::Ge: {
                    if (is_signed) {
                        pred_result = lhs_i >= rhs_i;
                    } else {
                        pred_result = static_cast<std::uint64_t>(lhs_i) >= static_cast<std::uint64_t>(rhs_i);
                    }
                    break;
                }
                default: std::unreachable();
            }

            const auto boolean = pred_result ? Value::true_value() : Value::false_value();
            return merge_state(icmp, LatticeValue::constant(boolean));
        }

        bool process_cond_branch(const CondBranch* cond) {
            const auto cond_state = lattice_of_operand(cond->condition());
            if (cond_state.kind == LatticeKind::Constant) {
                const auto cond_value = as_condition_value(cond_state.value);
                if (!cond_value.has_value()) {
                    return false;
                }
                const auto* target = cond_value.value() ? cond->on_true() : cond->on_false();
                return m_executable_blocks.emplace(target).second;
            }

            const auto ins_true = m_executable_blocks.emplace(cond->on_true()).second;
            const auto ins_false = m_executable_blocks.emplace(cond->on_false()).second;
            return ins_true || ins_false;
        }

        void rewrite_constants() const {
            for (const auto& [inst, state]: m_states) {
                if (state.kind != LatticeKind::Constant) {
                    continue;
                }

                std::vector<const Instruction*> users;
                users.reserve(inst->users().size());
                for (const auto* user: inst->users()) {
                    users.push_back(user);
                }

                for (const auto* user: users) {
                    const auto operands = user->operands();
                    for (std::size_t idx = 0; idx < operands.size(); ++idx) {
                        const auto& op = operands[idx];
                        if (!op.is<ValueInstruction*>()) {
                            continue;
                        }
                        if (op.get<ValueInstruction*>() != inst) {
                            continue;
                        }

                        const_cast<Instruction*>(user)->update_operand(idx, state.value);
                    }
                }
            }
        }

        void simplify_branches() const {
            std::vector<const CondBranch*> foldable;

            for (const auto& bb : m_fn.basic_blocks()) {
                if (!m_executable_blocks.contains(&bb)) {
                    continue;
                }

                for (const auto& inst : bb.instructions()) {
                    const auto* cond = dynamic_cast<const CondBranch*>(&inst);
                    if (cond == nullptr) {
                        continue;
                    }

                    const auto cond_state = lattice_of_operand(cond->condition());
                    if (cond_state.kind != LatticeKind::Constant || !as_condition_value(cond_state.value).has_value()) {
                        continue;
                    }

                    foldable.push_back(cond);
                }
            }

            for (const auto* cond : foldable) {
                auto* owner = cond->owner();
                const auto cond_state = lattice_of_operand(cond->condition());
                const auto cond_value = as_condition_value(cond_state.value);
                assertion(cond_state.kind == LatticeKind::Constant && cond_value.has_value(), "must be constant");

                const auto* target = cond_value.value() ? cond->on_true() : cond->on_false();
                const auto term = Terminator::from(cond);
                assertion(term.has_value(), "must be terminator");
                for (const auto* succ : term->targets()) {
                    const_cast<BasicBlock*>(succ)->remove_predecessor(owner);
                }

                owner->remove_instruction(cond);
                owner->ins(Branch::br(const_cast<BasicBlock*>(target)));
            }
        }

        void remove_dead_blocks() const {
            std::vector<const BasicBlock*> dead_blocks;
            for (const auto& bb : m_fn.basic_blocks()) {
                if (m_executable_blocks.contains(&bb)) {
                    continue;
                }
                dead_blocks.push_back(&bb);
            }

            for (const auto* dead_block : dead_blocks) {
                const auto dead_term = dead_block->last();
                for (const auto* succ : dead_term.targets()) {
                    auto* succ_mut = const_cast<BasicBlock*>(succ);
                    for (auto& inst : succ_mut->instructions()) {
                        auto* phi = Phi::cast(&inst);
                        if (phi == nullptr) {
                            break;
                        }

                        phi->remove_incoming_from(dead_block);
                    }
                    succ_mut->remove_predecessor(dead_block);
                }

                m_fn.remove(dead_block);
            }
        }

        void remove_dead_instructions() const {
            std::deque<const ValueInstruction*> worklist;

            for (const auto& bb : m_fn.basic_blocks()) {
                if (!m_executable_blocks.contains(&bb)) {
                    continue;
                }

                for (const auto& inst : bb.instructions()) {
                    const auto* value_inst = ValueInstruction::cast(&inst);
                    if (value_inst == nullptr) {
                        continue;
                    }
                    if (!is_removable_dead_instruction(value_inst)) {
                        continue;
                    }
                    if (!value_inst->users().empty()) {
                        continue;
                    }

                    worklist.push_back(value_inst);
                }
            }

            while (!worklist.empty()) {
                const auto* inst = worklist.front();
                worklist.pop_front();

                auto* owner = inst->owner();
                if (!m_executable_blocks.contains(owner)) {
                    continue;
                }
                if (!is_removable_dead_instruction(inst)) {
                    continue;
                }
                if (!inst->users().empty()) {
                    continue;
                }

                std::vector<const ValueInstruction*> operand_defs;
                for (const auto& operand : inst->operands()) {
                    if (!operand.is<ValueInstruction*>()) {
                        continue;
                    }

                    const auto* def = operand.get<ValueInstruction*>();
                    if (!is_removable_dead_instruction(def)) {
                        continue;
                    }
                    operand_defs.push_back(def);
                }

                owner->remove_instruction(inst);

                for (const auto* def : operand_defs) {
                    if (def->users().empty()) {
                        worklist.push_back(def);
                    }
                }
            }
        }

        FunctionData& m_fn;
        std::unordered_set<const BasicBlock*> m_executable_blocks;
        std::unordered_map<const ValueInstruction*, LatticeValue> m_states;
    };
}

void Sccp::run() noexcept {
    SCCPEval eval(m_fn);
    eval.run();
}

Sccp Sccp::create(FunctionData &fn) noexcept {
    return Sccp(fn);
}

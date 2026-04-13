#include "EscapeAnalysis.h"

#include "mir/instruction/Alloc.h"
#include "mir/instruction/FieldAccess.h"
#include "mir/instruction/GetElementPtr.h"
#include "mir/instruction/GetFieldPtr.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/Unary.h"

namespace {
    class InstructionEscapeStateAnalysis final: public Visitor {
    public:
        explicit InstructionEscapeStateAnalysis(std::unordered_map<const ValueInstruction*, EscapeState>& escape_state) noexcept:
            m_escape_state(escape_state) {}

        void run(const Instruction &inst) noexcept {
            inst.visit(*this);
        }

    private:
        [[nodiscard]]
        EscapeState get_state(const ValueInstruction* value_inst) const noexcept {
            const auto escape_state = m_escape_state.find(value_inst);
            if (escape_state == m_escape_state.end()) {
                return EscapeState::UNKNOWN;
            }

            return escape_state->second;
        }

        [[nodiscard]]
        EscapeState get_state(const Value& operand) const noexcept {
            if (!operand.isa(ptr_type_inst())) {
                return EscapeState::UNKNOWN;
            }

            const auto value_inst = operand.get<ValueInstruction*>();
            return get_state(value_inst);
        }

        void set_state(const ValueInstruction* value_inst, const EscapeState state) const noexcept {
            assertion(PointerType::cast(value_inst->type()) != nullptr, "must be");
            m_escape_state.emplace(value_inst, state);
        }

        void set_state(const Value& operand, const EscapeState state) const noexcept {
            if (!operand.isa(ptr_type_inst())) {
                return;
            }

            const auto value_inst = operand.get<ValueInstruction*>();
            set_state(value_inst, state);
        }

        [[nodiscard]]
        EscapeState join_with(const Value& operand, const EscapeState state) const noexcept {
            return join(get_state(operand), state);
        }

        [[nodiscard]]
        EscapeState join_with(const ValueInstruction* operand, const EscapeState state) const noexcept {
            return join(get_state(operand), state);
        }

        void accept(Binary *inst) override {}

        void load(const Unary* load) const {
            const auto& operand = load->operand();
            if (!operand.isa(ptr_type_inst())) {
                set_state(operand, EscapeState::NOESCAPE);
            } else {
                set_state(operand, EscapeState::UNKNOWN);
            }
        }

        void ptr2int(const Unary* ptr2int) const {
            const auto& operand = ptr2int->operand();
            const auto new_operand_state = join_with(operand, EscapeState::NOESCAPE);
            set_state(operand, new_operand_state);
        }

        void accept(Unary *inst) override {
            switch (inst->op()) {
                case UnaryOp::Load:    load(inst); break;
                case UnaryOp::Ptr2Int: ptr2int(inst); break;
                default: break;
            }
        }

        void accept(Branch *branch) override {}

        void accept(CondBranch *cond_branch) override {}

        void any_call(const std::span<Value const> args) const noexcept {
            for (const auto& arg : args) {
                if (!arg.isa(ptr_type_inst())) {
                    continue;
                }

                const auto value_inst = arg.get<ValueInstruction*>();
                const auto new_arg_state = join_with(value_inst, EscapeState::ARGUMENT_ESCAPED);
                set_state(value_inst, new_arg_state);
            }
        }

        void accept(Call *inst) override {
            any_call(inst->args());
        }

        void accept(TupleCall *inst) override {
            any_call(inst->args());
        }

        void accept(Return *inst) override {}

        void accept(ReturnValue *inst) override {}

        void accept(Switch *inst) override {}

        void accept(VCall *call) override {
            any_call(call->args());
        }

        void accept(IVCall *call) override {
            any_call(call->args());
        }

        void accept(Phi *inst) override {}

        void accept(Store *store) override {
            const auto new_pointer_state = join_with(store->pointer(), EscapeState::NOESCAPE);
            set_state(store->pointer(), new_pointer_state);

            const auto& value = store->value();
            if (!value.isa(ptr_type_inst())) {
                return;
            }

            const auto value_inst = value.get<ValueInstruction*>();
            const auto new_value_state = join_with(value_inst, EscapeState::FIELD_ESCAPED);
            set_state(value_inst, new_value_state);
        }

        void accept(Alloc *alloc) override {
            set_state(alloc, EscapeState::NOESCAPE);
        }

        void accept(IcmpInstruction *icmp) override {}

        void accept(FcmpInstruction *fcmp) override {}

        void field_access(const FieldAccess* fa) const noexcept {
            const auto& source = fa->pointer();
            const auto new_source_state = join_with(source, EscapeState::FIELD_ESCAPED);
            set_state(source, new_source_state);
        }

        void accept(GetElementPtr *gep) override {
            field_access(gep);
        }

        void accept(GetFieldPtr *gfp) override {
            field_access(gfp);
        }

        void accept(Select *select) override {}

        void accept(IntDiv *div) override {}

        void accept(Projection *proj) override {}

        std::unordered_map<const ValueInstruction*, EscapeState>& m_escape_state;
    };
}

void EscapeAnalysis::run() {
    InstructionEscapeStateAnalysis inst_escape_state_analysis(m_escape_state);
    for (const auto& bb: m_ordering) {
        for (const auto& inst: bb->instructions()) {
            inst_escape_state_analysis.run(inst);
        }
    }
}
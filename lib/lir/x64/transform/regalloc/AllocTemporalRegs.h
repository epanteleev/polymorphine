#pragma once

#include "lir/x64/module/LIRBlock.h"

namespace details {
    class AllocTemporalRegs final: public LIRVisitor {
    public:
        static std::uint8_t allocate(aasm::SymbolTable& symbol_tab, const LIRInstructionBase* inst) {
            AllocTemporalRegs emitter(symbol_tab);
            const_cast<LIRInstructionBase*>(inst)->visit(emitter);
            const auto s = emitter.m_temp_counter.used_gp_regs();
            return s;
        }

    private:
        class TemporalRegsCounter final {
        public:
            [[nodiscard]]
            aasm::GPReg gp_temp1() const noexcept {
                m_used_gp_regs |= 1 << 0;
                return aasm::rax;
            }

            [[nodiscard]]
            aasm::GPReg gp_temp2() const noexcept {
                m_used_gp_regs |= 1 << 1;
                return aasm::rax;
            }

            std::uint8_t used_gp_regs() const noexcept {
                return std::popcount(m_used_gp_regs);
            }

        private:
            mutable std::uint8_t m_used_gp_regs{};
        };

        explicit AllocTemporalRegs(aasm::SymbolTable& symbol_tab) noexcept:
            m_symbol_tab(symbol_tab) {}

        [[nodiscard]]
        GPOp convert_to_gp_op(const LIROperand &val) const;

        void gen(const LIRVal &out) override {}

        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override;

        void div_u(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override;

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void setcc_i(const LIRVal &out, aasm::CondType cond_type) override {}

        void cmov_i(aasm::CondType cond_type, const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override;

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}

        void movzx_i(const LIRVal &out, const LIROperand &in) override {}

        void movsx_i(const LIRVal &out, const LIROperand &in) override {}

        void trunc_i(const LIRVal &out, const LIROperand &in) override {}

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override;

        void neg_i(const LIRVal &out, const LIROperand &in) override {}

        void not_i(const LIRVal &out, const LIROperand &in) override {}

        void mov_i(const LIROperand &in1, const LIROperand &in2) override;

        void mov_by_idx_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) override;

        void store_by_offset_i(const LIROperand  &pointer, const LIROperand &index, const LIROperand &value) override;

        void store_i(const LIRVal &pointer, const LIROperand &value) override;

        void up_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void down_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void copy_i(const LIRVal &out, const LIROperand &in) override {}

        void load_i(const LIRVal &out, const LIRVal &pointer) override {}

        void load_by_idx_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override;

        void read_by_offset_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override {}

        void lea_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override {}

        void jmp(const LIRBlock *bb) override {}

        void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, FunctionBind linkage) override {}

        void vcall(const std::string_view, std::span<LIRVal const>, FunctionBind) override {}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(std::span<LIRVal const> ret_values) override {}

        TemporalRegsCounter m_temp_counter{};
        aasm::SymbolTable& m_symbol_tab;
    };
}

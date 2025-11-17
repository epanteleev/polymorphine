#include "AllocTemporalRegs.h"

#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"


#include "lir/x64/asm/operand/GPOp.h"
#include "lir/x64/asm/operand/XVReg.h"
#include "lir/x64/asm/EmptyEmitter.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/emitters/CmpGPEmit.h"
#include "lir/x64/asm/emitters/DivIntEmit.h"
#include "lir/x64/asm/emitters/DivUIntEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovGPEmit.h"
#include "lir/x64/asm/emitters/StoreGPEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"
#include "lir/x64/asm/map/LIROperandMapping.h"
#include "lir/x64/module/LIRBlock.h"

namespace details {
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

    class AllocTemporalRegsForInstruction final: public LIRVisitor, public LIROperandMapping {
    public:
        explicit AllocTemporalRegsForInstruction(aasm::SymbolTable& symbol_tab) noexcept:
            LIROperandMapping(symbol_tab) {}

        void gen(const LIRVal &out) override{}

        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void div_i(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override {
            const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);

            DivIntEmit emitter(m_temp_counter, m_as, in1.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void div_u(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override {
            const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            DivUIntEmit emitter(m_temp_counter, m_as, in1.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void setcc_i(const LIRVal &out, aasm::CondType cond_type) override{};

        void cmov_i(aasm::CondType cond_type, const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            CMovGPEmit emitter(m_temp_counter, m_as, cond_type, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override{}

        void movzx_i(const LIRVal &out, const LIROperand &in) override{}

        void movsx_i(const LIRVal &out, const LIROperand &in) override{}

        void trunc_i(const LIRVal &out, const LIROperand &in) override{}

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            CmpGPEmit emitter(m_temp_counter, m_as, in1.size());
            emitter.apply(in1_reg, in2_reg);
        }

        void neg_i(const LIRVal &out, const LIROperand &in) override{}

        void not_i(const LIRVal &out, const LIROperand &in) override{}

        void mov_i(const LIROperand &in1, const LIROperand &in2) override {
            const auto in1_reg = convert_to_gp_op(in1);
            const auto add_opt = in1_reg.as_address();
            assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

            const auto in2_op = convert_to_gp_op(in2);
            MovGPEmit emitter(m_temp_counter, m_as, in1.size());
            emitter.apply(add_opt.value(), in2_op);
        }

        void mov_by_idx_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) override {
            const auto out_reg = pointer.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto in2_op = convert_to_gp_op(in);
            MovByIdxIntEmit emitter(m_temp_counter, m_as, in.size());
            emitter.apply(out_reg, index_op, in2_op);
        }

        void store_by_offset_i(const LIROperand &pointer, const LIROperand &index, const LIROperand &value) override {
            const auto out_vreg = convert_to_gp_op(pointer);
            const auto out_addr = out_vreg.as_address();
            assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

            const auto index_op = convert_to_gp_op(index);
            const auto value_op = convert_to_gp_op(value);
            StoreOnStackGPEmit emitter(m_temp_counter, m_as, value.size());
            emitter.apply(out_addr.value(), index_op, value_op);
        }

        void store_i(const LIRVal &pointer, const LIROperand &value) override {
            const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
            const auto value_op = convert_to_gp_op(value);
            StoreGPEmit emitter(m_temp_counter, m_as, value.size());
            emitter.apply(pointer_reg, value_op);
        }

        void up_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void down_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override{}

        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override{}

        void copy_i(const LIRVal &out, const LIROperand &in) override{}

        void load_i(const LIRVal &out, const LIRVal &pointer) override{}

        void load_by_idx_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);
            LoadByIdxIntEmit emitter(m_temp_counter, m_as, out.size());
            emitter.apply(out_reg, pointer_op, index_op);
        };

        void read_by_offset_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override{}

        void lea_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override{}

        void copy_f(const LIRVal &out, const LIROperand &in) override{}

        void load_f(const LIRVal &out, const LIRVal &pointer) override{}

        void mov_f(const LIROperand &in1, const LIROperand &in2) override{}

        void cmp_f(const LIROperand &in1, const LIROperand &in2) override{}

        void add_f(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void jmp(const LIRBlock *bb) override{}

        void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override{}

        void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override {}

        void vcall(std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override{}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override{}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override{}

        void ret(std::span<LIRVal const> ret_values) override{}

        TemporalRegsCounter m_temp_counter{};
        EmptyEmitter m_as{};
    };

    std::uint8_t AllocTemporalRegs::allocate(aasm::SymbolTable &symbol_tab, const LIRInstructionBase *inst) {
        AllocTemporalRegsForInstruction emitter(symbol_tab);
        const_cast<LIRInstructionBase*>(inst)->visit(emitter);
        return emitter.m_temp_counter.used_gp_regs();
    }
}
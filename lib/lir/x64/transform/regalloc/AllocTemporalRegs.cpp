#include "AllocTemporalRegs.h"

#include "lir/x64/asm/operand/GPOp.h"
#include "lir/x64/asm/operand/XOp.h"
#include "lir/x64/asm/operand/XVReg.h"

#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinarySrcAddrVisitor.h"
#include "lir/x64/asm/visitors/XBinaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryOutVisitor.h"

#include "lir/x64/asm/EmptyEmitter.h"
#include "lir/x64/asm/emitters/AddIntEmit.h"
#include "lir/x64/asm/emitters/AddFloatEmit.h"
#include "lir/x64/asm/emitters/SubIntEmit.h"
#include "lir/x64/asm/emitters/XorIntEmit.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/emitters/CmpGPEmit.h"
#include "lir/x64/asm/emitters/DivIntEmit.h"
#include "lir/x64/asm/emitters/DivUIntEmit.h"
#include "lir/x64/asm/emitters/TruncIntEmit.h"
#include "lir/x64/asm/emitters/CopyGPEmit.h"
#include "lir/x64/asm/emitters/LoadGPEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovGPEmit.h"
#include "lir/x64/asm/emitters/MovsxIntEmit.h"
#include "lir/x64/asm/emitters/MovzxIntEmit.h"
#include "lir/x64/asm/emitters/StoreGPEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"
#include "lir/x64/asm/emitters/LoadFromStackGPEmit.h"
#include "lir/x64/asm/emitters/LeaGPEmit.h"
#include "lir/x64/asm/emitters/CopyFloatEmit.h"
#include "lir/x64/asm/emitters/CmpFloatEmit.h"
#include "lir/x64/asm/map/LIROperandMapping.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/asm/map/LIRInstuctionMapping.h"

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

        [[nodiscard]]
        aasm::XmmReg xmm_temp1() const noexcept {
            m_used_xmm_regs |= 1 << 0;
            return aasm::xmm0;
        }

        [[nodiscard]]
        std::uint8_t used_gp_regs() const noexcept {
            return std::popcount(m_used_gp_regs);
        }

        [[nodiscard]]
        std::uint8_t used_xmm_regs() const noexcept {
            return std::popcount(m_used_xmm_regs);
        }

    private:
        mutable std::uint8_t m_used_gp_regs{};
        mutable std::uint8_t m_used_xmm_regs{};
    };

    class AllocTemporalRegsForInstruction final: public LIRInstructionMapping<TemporalRegsCounter, EmptyEmitter> {
    public:
        explicit AllocTemporalRegsForInstruction(const TemporalRegsCounter& temp_counter, EmptyEmitter& as, aasm::SymbolTable& symbol_tab) noexcept:
            LIRInstructionMapping(temp_counter, as, symbol_tab) {}

        void gen(const LIRVal &out) override{}

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override{}

        void up_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void down_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}

        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override{}

        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override{}

        void jmp(const LIRBlock *bb) override{}

        void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override{}

        void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override {}

        void vcall(std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override{}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override{}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override{}

        void ret(std::span<LIRVal const> ret_values) override{}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override{}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void setcc_i(const LIRVal &out, aasm::CondType cond_type) override {}

        void neg_i(const LIRVal &out, const LIROperand &in) override {}

        void not_i(const LIRVal &out, const LIROperand &in) override {}

        void load_f(const LIRVal &out, const LIRVal &pointer) override {}

        void mov_f(const LIROperand &in1, const LIROperand &in2) override {}
    };

    std::pair<std::uint8_t, std::uint8_t> AllocTemporalRegs::allocate(aasm::SymbolTable &symbol_tab, const LIRInstructionBase *inst) {
        constexpr TemporalRegsCounter temp_counter{};
        static constinit EmptyEmitter as{};
        AllocTemporalRegsForInstruction emitter(temp_counter, as, symbol_tab);
        const_cast<LIRInstructionBase*>(inst)->visit(emitter);
        return std::make_pair(temp_counter.used_gp_regs(), temp_counter.used_xmm_regs());
    }
}
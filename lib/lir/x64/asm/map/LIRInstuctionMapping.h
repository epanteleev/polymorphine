#pragma once
#include "LIROperandMapping.h"
#include "lir/x64/instruction/LIRVisitor.h"

#include "lir/x64/asm/operand/GPOp.h"
#include "lir/x64/asm/operand/XOp.h"
#include "lir/x64/asm/operand/XVReg.h"

#include "asm/x64/reg/AnyRegSet.h"
#include "lir/x64/asm/operand/XOp.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/asm/visitors/GPBinarySrcAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"
#include "lir/x64/asm/visitors/XBinaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/XBinaryVisitorWithGP.h"
#include "lir/x64/asm/visitors/XBinaryVisitorXOut.h"
#include "lir/x64/asm/visitors/XBinaryAddrVisitor.h"
#include "lir/x64/asm/visitors/XUnaryGpOutVisitor.h"
#include "lir/x64/asm/visitors/XUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryXmmOutVisitor.h"
#include "lir/x64/asm/visitors/XBinarySrcAddrVisitor.h"

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
#include "lir/x64/asm/emitters/MovByIdxFloatEmit.h"
#include "lir/x64/asm/emitters/CmpFloatEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxFloatEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackXmmEmit.h"
#include "lir/x64/asm/emitters/StoreXmmEmit.h"
#include "lir/x64/asm/emitters/MovFloatEmit.h"
#include "lir/x64/asm/emitters/LoadFloatEmit.h"
#include "lir/x64/asm/emitters/LoadFromStackFloatEmit.h"
#include "lir/x64/asm/emitters/CvtFp2IntEmit.h"
#include "lir/x64/asm/emitters/CvtInt2FpEmit.h"


namespace details {
    template<typename TemporalRegStorage, typename AsmEmit>
    class LIRInstructionMapping: public LIRVisitor, public LIROperandMapping {
    public:
        explicit LIRInstructionMapping(const TemporalRegStorage &reg_storage, AsmEmit& asm_emit, aasm::SymbolTable& symbol_table) noexcept:
            LIROperandMapping(symbol_table),
            m_temp_regs(reg_storage),
            m_as(asm_emit) {}

        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            AddIntEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            SubIntEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void div_i(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);

            DivIntEmit emitter(m_temp_regs, m_as, in1.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void div_u(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);

            DivUIntEmit emitter(m_temp_regs, m_as, in1.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            XorIntEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void cmov_i(aasm::CondType cond_type, const LIRVal &out, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            CMovGPEmit emitter(m_temp_regs, m_as, cond_type, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void movzx_i(const LIRVal &out, const LIROperand &in) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto pointer_reg = convert_to_gp_op(in);
            MovzxGPEmit emitter(m_temp_regs, m_as, out.size(), in.size());
            emitter.apply(out_reg, pointer_reg);
        }

        void movsx_i(const LIRVal &out, const LIROperand &in) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto pointer_reg = convert_to_gp_op(in);
            MovsxGPEmit emitter(m_temp_regs, m_as, out.size(), in.size());
            emitter.apply(out_reg, pointer_reg);
        }

        void trunc_i(const LIRVal &out, const LIROperand &in) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto pointer_reg = convert_to_gp_op(in);
            TruncGPEmit emitter(m_temp_regs, m_as, out.size(), in.size());
            emitter.emit(out_reg, pointer_reg);
        }

        void cmp_i(const LIROperand &in1, const LIROperand &in2) final {
            const auto in1_reg = convert_to_gp_op(in1);
            const auto in2_reg = convert_to_gp_op(in2);
            CmpGPEmit emitter(m_temp_regs, m_as, in1.size());
            emitter.apply(in1_reg, in2_reg);
        }

        void mov_i(const LIROperand &in1, const LIROperand &in2) final {
            const auto in1_reg = convert_to_gp_op(in1);
            const auto add_opt = in1_reg.as_address();
            assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

            const auto in2_op = convert_to_gp_op(in2);
            MovGPEmit emitter(m_temp_regs, m_as, in1.size());
            emitter.apply(add_opt.value(), in2_op);
        }

        void mov_by_idx_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) final {
            const auto out_reg = pointer.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto in2_op = convert_to_gp_op(in);

            MovByIdxIntEmit emitter(m_temp_regs, m_as, in.size());
            emitter.apply(out_reg, index_op, in2_op);
        }

        void store_by_offset_i(const LIROperand &pointer, const LIROperand &index, const LIROperand &value) final {
            const auto out_vreg = convert_to_gp_op(pointer);
            const auto out_addr = out_vreg.as_address();
            assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

            const auto index_op = convert_to_gp_op(index);
            const auto value_op = convert_to_gp_op(value);
            StoreOnStackGPEmit emitter(m_temp_regs, m_as, value.size());
            emitter.apply(out_addr.value(), index_op, value_op);
        }

        void store_i(const LIRVal &pointer, const LIROperand &value) final {
            const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
            const auto value_op = convert_to_gp_op(value);
            StoreGPEmit emitter(m_temp_regs, m_as, value.size());
            emitter.apply(pointer_reg, value_op);
        }

        void copy_i(const LIRVal &out, const LIROperand &in) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            CopyGPEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, convert_to_gp_op(in));
        }

        void load_i(const LIRVal &out, const LIRVal &pointer) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
            LoadGPEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, pointer_reg);
        }

        void load_by_idx_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);

            LoadByIdxIntEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, pointer_op, index_op);
        }

        void read_by_offset_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);
            const auto pointer_addr = pointer_op.as_address();
            assertion(pointer_addr.has_value(), "Invalid LIRVal for load_from_stack_i");

            LoadFromStackGPEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, index_op, pointer_addr.value());
        }

        void lea_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) final {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);

            LeaAddrGPEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, index_op, pointer_op.as_address().value());
        }

        void copy_f(const LIRVal &out, const LIROperand &in) final {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            CopyFloatEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, convert_to_x_op(in));
        }

        void cmp_f(const FcmpOrdering ord, const LIROperand &in1, const LIROperand &in2) final {
            const auto in1_reg = convert_to_x_op(in1);
            const auto in2_reg = convert_to_x_op(in2);
            CmpFloatEmit emitter(ord, m_temp_regs, m_as, in1.size());
            emitter.apply(in1_reg, in2_reg);
        }

        void add_f(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) final {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            const auto in1_reg = convert_to_x_op(in1);
            const auto in2_reg = convert_to_x_op(in2);
            AddFloatEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, in1_reg, in2_reg);
        }

        void mov_f(const LIROperand &in1, const LIROperand &in2) final {
            const auto in1_reg = convert_to_gp_op(in1);
            const auto add_opt = in1_reg.as_address();
            assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

            const auto in2_op = convert_to_x_op(in2);
            MovFloatEmit emitter(m_temp_regs, m_as, in1.size());
            emitter.apply(add_opt.value(), in2_op);
        }

        void mov_by_idx_f(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) override {
            const auto out_reg = pointer.assigned_reg().to_gp_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto in2_op = convert_to_x_op(in);

            MovByIdxFloatEmit emitter(m_temp_regs, m_as, in.size());
            emitter.apply(out_reg, index_op, in2_op);
        }

        void load_by_idx_f(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);

            LoadByIdxFloatEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, pointer_op, index_op);
        }

        void store_by_offset_f(const LIROperand &pointer, const LIROperand &index, const LIROperand &value) override {
            const auto out_vreg = convert_to_gp_op(pointer);
            const auto out_addr = out_vreg.as_address();
            assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

            const auto index_op = convert_to_gp_op(index);
            const auto value_op = convert_to_x_op(value);
            StoreOnStackXmmEmit emitter(m_temp_regs, m_as, value.size());
            emitter.apply(out_addr.value(), index_op, value_op);
        }

        void read_by_offset_f(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) override {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            const auto index_op = convert_to_gp_op(index);
            const auto pointer_op = convert_to_gp_op(pointer);
            const auto pointer_addr = pointer_op.as_address();
            assertion(pointer_addr.has_value(), "Invalid LIRVal for load_from_stack_i");

            LoadFromStackFloatEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, index_op, pointer_addr.value());
        }

        void cvtfp2int(const LIRVal &out, const LIROperand &in) override {
            const auto out_reg = out.assigned_reg().to_gp_op().value();
            const auto in_reg = convert_to_x_op(in);
            CvtFp2IntEmit emitter(m_temp_regs, m_as, in.size(), out.size());
            emitter.apply(out_reg, in_reg);
        }

        void cvtint2fp(const LIRVal &out, const LIROperand &in) override {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            const auto in_reg = convert_to_gp_op(in);
            CvtInt2FpEmit emitter(m_temp_regs, m_as, in.size(), out.size());
            emitter.apply(out_reg, in_reg);
        }

        void cvtuint2fp(const LIRVal &out, const LIROperand &in) override {
            unimplemented();
        }

        void store_f(const LIRVal &pointer, const LIROperand &value) final {
            const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
            const auto value_op = convert_to_x_op(value);
            StoreXmmEmit emitter(m_temp_regs, m_as, value.size());
            emitter.apply(pointer_reg, value_op);
        }

        void load_f(const LIRVal &out, const LIRVal &pointer) final {
            const auto out_reg = out.assigned_reg().to_xmm_op().value();
            const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
            LoadFloatEmit emitter(m_temp_regs, m_as, out.size());
            emitter.apply(out_reg, pointer_reg);
        }

    protected:
        const TemporalRegStorage& m_temp_regs;
        AsmEmit& m_as;
    };
}

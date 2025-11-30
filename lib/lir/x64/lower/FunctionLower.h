#pragma once

#include <ranges>

#include "mir/mir.h"
#include "base/analysis/AnalysisPassManagerBase.h"
#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/module/LIRFuncData.h"

/**
 * FunctionLower is responsible for lowering the MIR function to LIR.
 * It traverses the function's basic blocks in a domination order.
 */
class FunctionLower final: public Visitor {
    FunctionLower(LIRFuncData&& obj_function, const FunctionData &function, const Ordering<BasicBlock>& dom_ordering, GlobalData& global_data, const call_conv::CallConvProvider* call_conv) noexcept:
        m_obj_function(std::move(obj_function)),
        m_function(function),
        m_dom_ordering(dom_ordering),
        m_global_data(global_data),
        m_call_conv(call_conv),
        m_bb(m_obj_function.first()) {}

public:
    void run() {
        setup_arguments();
        setup_bb_mapping();
        traverse_instructions();
        finalize_parallel_copies();
    }

    static FunctionLower create(AnalysisPassManagerBase<FunctionData> *cache, const FunctionData *data, GlobalData& global_data, const call_conv::CallConvProvider* call_conv) {
        // It is assumed that bfs order guarantees domination order.
        const auto* bfs = cache->analyze<BFSOrderTraverseBase<FunctionData>>(data);
        return {create_lir_function(*data), *data, *bfs, global_data, call_conv};
    }

    LIRFuncData result() {
        return std::move(m_obj_function);
    }

private:
    static LIRFuncData create_lir_function(const FunctionData &function);

    void allocate_fixed_regs_for_arguments() const;

    void allocate_arguments_for_call(std::span<LIROperand const> args) const;

    void setup_gp_argument(std::size_t idx, const ArgumentValue& arg, const LIROperand& lir_arg);

    void setup_arguments();

    void traverse_instructions();

    void setup_bb_mapping();

    void finalize_parallel_copies() const noexcept;

    void accept(Binary *inst) override;

    void accept(Unary *inst) override;

    void accept(Branch *branch) override;

    void accept(CondBranch *cond_branch) override;

    void accept(Call *call) override;

    void accept(Return *inst) override;

    void accept(ReturnValue *inst) override;

    void accept(Switch *inst) override {
        unimplemented();
    }

    void accept(VCall *call) override;

    void accept(IVCall *call) override {
        unimplemented();
    }

    void accept(Phi *inst) override;

    void accept(Store *store) override;

    void accept(Alloc *alloc) override;

    void accept(IcmpInstruction *icmp) override;

    void accept(FcmpInstruction *fcmp) override;

    void accept(GetElementPtr *gep) override;

    void accept(GetFieldPtr *gfp) override;

    void accept(Select *select) override;

    void accept(IntDiv *div) override;

    void accept(Projection *proj) override {}

    void lower_load(const Unary *inst);
    LIRVal lower_primitive_type_argument(const Value& arg);
    std::vector<LIROperand> lower_function_prototypes(std::span<const Value> operands, const FunctionPrototype& proto);

    void make_setcc(const ValueInstruction *inst, aasm::CondType cond_type);

    void try_schedule_late(const Value& cond);
    void try_schedule_late(ValueInstruction *inst);
    void schedule_late(ValueInstruction *inst);

    LIROperand lower_global_cst(const GlobalValue &global);
    LIRVal lower_return_value(const PrimitiveType *ret_type, const Value &val, aasm::GPReg fixed_reg);
    LIROperand make_fp_constant(const Type &type, double val);
    LIROperand get_lir_operand(const Value& val);
    LIRVal get_lir_val(const Value& val);

    template <IsLocalValueType T>
    void memorize(const T* val, const LIROperand& lir_val) {
        m_value_mapping.emplace(UsedValue::from(val), lir_val);
    }

    std::size_t m_cst_index{};
    LIRFuncData m_obj_function;
    const FunctionData& m_function;
    const Ordering<BasicBlock>& m_dom_ordering;
    GlobalData& m_global_data;
    const call_conv::CallConvProvider* m_call_conv;

    LIRBlock* m_bb;
    std::unordered_map<const BasicBlock*, LIRBlock*> m_bb_mapping;
    UsedValueMap<LIROperand> m_value_mapping;
    // Inserted parallel copies in the current function for late handling.
    std::unordered_set<LIRBlock*> m_parallel_copy_owners;
    // Temporal storage for late scheduled instructions.
    std::unordered_set<ValueInstruction*> m_late_schedule_instructions;
};


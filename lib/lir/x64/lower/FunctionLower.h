#pragma once

#include <ranges>

#include "base/analysis/AnalysisPassManagerBase.h"
#include "mir/mir.h"

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/module/LIRFuncData.h"

/**
 * FunctionLower is responsible for lowering the MIR function to LIR.
 * It traverses the function's basic blocks in a domination order.
 */
class FunctionLower final: public Visitor {
    FunctionLower(std::unique_ptr<LIRFuncData>&& obj_function, const FunctionData &function, const Ordering<BasicBlock>& dom_ordering) noexcept:
        m_obj_function(std::move(obj_function)),
        m_function(function),
        m_dom_ordering(dom_ordering),
        m_bb(m_obj_function->first()) {}

public:
    void run() {
        setup_arguments();
        setup_bb_mapping();
        traverse_instructions();
    }

    static FunctionLower create(AnalysisPassManagerBase<FunctionData> *cache, const FunctionData *data) {
        // It is assumed that bfs order guarantees domination order.
        const auto* bfs = cache->analyze<BFSOrderTraverseBase<FunctionData>>(data);
        return {create_lir_function(*data), *data, *bfs};
    }

    std::unique_ptr<LIRFuncData> result() {
        return std::move(m_obj_function);
    }

private:
    static std::unique_ptr<LIRFuncData> create_lir_function(const FunctionData &function) {
        std::vector<LIRArg> args;
        args.reserve(function.args().size());

        for (auto [idx, varg]: std::ranges::views::enumerate(function.args())) {
            args.emplace_back(idx, varg.type()->size_of());
        }

        return std::make_unique<LIRFuncData>(function.name(), std::move(args));
    }

    void setup_arguments() {
        for (const auto& [arg, lir_arg]: std::ranges::zip_view(m_function.args(), m_obj_function->args())) {
            memorize(&arg, lir_arg);
        }
    }

    void traverse_instructions() {
        for (const auto &bb: m_dom_ordering) {
            m_bb = m_bb_mapping.at(bb);
            for (auto &inst: bb->instructions()) {
                inst.visit(*this);
            }
        }
    }

    void setup_bb_mapping() {
        for (const auto& bb: m_function.basic_blocks()) {
            if (&bb == m_function.first()) {
                m_bb_mapping.emplace(&bb, m_obj_function->first());
                continue;
            }

            auto lir_bb = m_obj_function->create_mach_block();
            m_bb_mapping.emplace(&bb, lir_bb);
        }
    }

    LIROperand get_lir_operand(const Value& val);

    LIRVal get_lir_val(const Value& val);

    void accept(Binary *inst) override;

    void accept(Unary *inst) override;

    void accept(Branch *branch) override;

    void accept(CondBranch *cond_branch) override;

    void accept(TerminateValueInstruction *inst) override {

    }

    void accept(Return *inst) override;

    void accept(ReturnValue *inst) override;

    void accept(Switch *inst) override {

    }

    void accept(VCall *call) override {

    }

    void accept(IVCall *call) override {

    }

    void accept(PhiInstruction *inst) override {

    }

    void accept(Store *store) override;

    void accept(Alloc *alloc) override;

    void accept(IcmpInstruction *icmp) override;

    void accept(GetElementPtr *gep) override {

    }

    void lower_flag2int(const Unary *inst);
    void lower_load(const Unary *inst);
    void make_setcc(const Unary *inst, LIRCondType cond_type);

    template <IsLocalValueType T>
    void memorize(const T* val, const LIRVal& lir_val) {
        const auto local = LocalValue::from(val);
        m_value_mapping.emplace(local, lir_val);
    }

    std::unique_ptr<LIRFuncData> m_obj_function;
    const FunctionData& m_function;
    const Ordering<BasicBlock>& m_dom_ordering;

    LIRBlock* m_bb;
    std::unordered_map<const BasicBlock*, LIRBlock*> m_bb_mapping;
    LocalValueMap<LIRVal> m_value_mapping;
};


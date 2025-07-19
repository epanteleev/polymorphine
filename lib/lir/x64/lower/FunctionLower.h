#pragma once

#include <ranges>

#include "base/analysis/AnalysisPassManagerBase.h"
#include "mir/value/LocalValue.h"
#include "mir/module/FunctionData.h"
#include "mir/value/LocalValueMap.h"

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/module/LIRFuncData.h"
#include "mir/analysis/Analysis.h"

class FunctionLower final: public Visitor {
    FunctionLower(std::unique_ptr<LIRFuncData>&& obj_function, const FunctionData &function, const Ordering<BasicBlock>& ordering) noexcept:
        m_obj_function(std::move(obj_function)),
        m_function(function),
        m_ordering(ordering),
        m_bb(m_obj_function->first()) {}

public:
    void run() {
        setup_arguments();
        setup_bb_mapping();
        traverse_instructions();
    }

    static FunctionLower create(AnalysisPassManagerBase<FunctionData> *cache, const FunctionData *data) {
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
            const auto local = LocalValue::from(&arg);
            m_value_mapping.emplace(local, lir_arg);
        }
    }

    void traverse_instructions() {
        for (const auto &bb: m_ordering) {
            m_bb = m_bb_mapping.at(bb);
            for (auto &inst: bb->instructions()) {
                inst.visit(*this);
            }
        }
    }

    void setup_bb_mapping() {
        for (const auto& bb: m_ordering) {
            if (bb == m_function.first()) {
                m_bb_mapping.emplace(bb, m_obj_function->first());
                continue;
            }

            auto lir_bb = m_obj_function->create_mach_block();
            m_bb_mapping.emplace(bb, lir_bb);
        }
    }

    LIROperand get_value_mapping(const Value& val);

    void accept(Binary *inst) override;

    void accept(Unary *inst) override {

    }

    void accept(Branch *branch) override;

    void accept(CondBranch *cond_branch) override {

    }

    void accept(TerminateValueInstruction *inst) override {

    }

    void accept(Return *inst) override {

    }

    void accept(ReturnValue *inst) override;

    void accept(Switch *inst) override {

    }

    void accept(VCall *call) override {

    }

    void accept(IVCall *call) override {

    }

    void accept(PhiInstruction *inst) override {

    }

    void accept(Store *store) override {

    }

    void accept(Alloc *alloc) override {

    }

    void accept(IcmpInstruction *icmp) override {

    }

    void accept(GetElementPtr *gep) override {

    }

    std::unique_ptr<LIRFuncData> m_obj_function;
    const FunctionData& m_function;
    const Ordering<BasicBlock>& m_ordering;

    LIRBlock* m_bb;
    std::unordered_map<const BasicBlock*, LIRBlock*> m_bb_mapping;
    LocalValueMap<LIRVal> m_value_mapping;
};


#pragma once

#include <ranges>

#include "mir/instruction/TerminateInstruction.h"
#include "mir/value/LocalValue.h"
#include "mir/module/FunctionData.h"
#include "mir/value/LocalValueMap.h"

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/module/ObjFuncData.h"
#include "lir/x64/instruction/LIRInstruction.h"

class FunctionLower final: public Visitor {
public:
    FunctionLower(ObjFuncData &obj_function, const FunctionData &function) noexcept
        : m_obj_function(obj_function), m_function(function) {}

    void run() {
        m_bb = m_obj_function.first();
        std::size_t idx{};
        for (const auto& [arg, lir_arg]: std::ranges::zip_view(m_function.args(), m_obj_function.args())) {
            const auto local = LocalValue::from(&arg);
            m_mapping.emplace(local, LIRVReg(arg.type()->size_of(), idx, &lir_arg));
            idx += 1;
        }

        for (const auto &bb: m_function.basic_blocks()) {
            for (const auto &inst: bb.instructions()) {
                const_cast<Instruction&>(inst).visit(*this); //TODO remove const_cast
            }
        }
    }

private:
    LIROperand get_mapping(const Value& val) {
        return LirCst::imm8(1);
    }

    void accept(Binary *inst) override {

    }

    void accept(Unary *inst) override {

    }

    void accept(Branch *branch) override {

    }

    void accept(CondBranch *cond_branch) override {

    }

    void accept(TerminateValueInstruction *inst) override {

    }

    void accept(Return *inst) override {

    }

    void accept(ReturnValue *inst) override {
        const auto ret_val = get_mapping(inst->ret_value());
        const auto copy = m_bb->inst(LIRInstruction::copy(ret_val));

        m_bb->inst(LIRReturn::ret(copy->def(0)));
    }

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

    ObjFuncData& m_obj_function;
    const FunctionData& m_function;
    MachBlock* m_bb{};
    LocalValueMap<LIRVReg> m_mapping;
};


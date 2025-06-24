#include "Lowering.h"

#include <ranges>

class FunctionLower final: public Visitor {
public:
    FunctionLower(ObjFuncData &obj_function, const FunctionData &function) noexcept
        : m_obj_function(obj_function), m_function(function) {}

    void run() {
        for (const auto &bb: m_function.basic_blocks()) {
            for (const auto &inst: bb.instructions()) {
                const_cast<Instruction&>(inst).visit(*this); //TODO remove const_cast
            }
        }
    }

private:
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
};


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        auto obj_func = std::make_unique<ObjFuncData>(func->name());
        FunctionLower lower(*obj_func.get(), *func);
        lower.run();

        m_obj_functions.emplace(func->name(), std::move(obj_func));
    }
}
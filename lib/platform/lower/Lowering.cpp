#include "Lowering.h"

#include <ranges>

#include "VregBuilder.hpp"
#include "instruction/TerminateInstruction.h"
#include "value/LocalValue.h"
#include "platform/lir/x64/LIRInstructionBase.h"

struct HashLocalVal final {
    [[nodiscard]]
    size_t operator()(const LocalValue& val) const noexcept {
        if (val.is<ArgumentValue>()) {
            return reinterpret_cast<std::size_t>(val.get<ArgumentValue>());
        }
        if (val.is<ValueInstruction>()) {
            return reinterpret_cast<std::size_t>(val.get<ValueInstruction>());
        }

        die("wrong variant");
    }
};

struct LocalValEqualTo final {
    bool operator()(const LocalValue& x, const LocalValue& y) const { return x == y; }
};


class FunctionLower final: public Visitor {
public:
    FunctionLower(ObjFuncData &obj_function, const FunctionData &function) noexcept
        : m_obj_function(obj_function), m_function(function) {}

    void run() {
        for (const auto& [arg, lir_arg]: std::ranges::zip_view(m_function.args(), m_obj_function.args())) {
            const auto local = LocalValue::from(&arg);
            auto vreg = m_builder.mk_vreg(&lir_arg);
            m_mapping.emplace(local, vreg);
        }

        m_bb = m_obj_function.first();
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

        m_bb->inst(LIRReturn::ret(copy->out(0)));
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
    std::unordered_map<LocalValue, VReg, HashLocalVal, LocalValEqualTo> m_mapping;
    VregBuilder m_builder{};
};


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        std::vector<LIRArg> args;
        for (auto [idx, varg]: std::ranges::views::enumerate(func->args())) {
            args.emplace_back(idx, varg.type()->size_of());
        }

        auto obj_func = std::make_unique<ObjFuncData>(func->name(), std::move(args));
        FunctionLower lower(*obj_func.get(), *func);
        lower.run();

        m_obj_functions.emplace(func->name(), std::move(obj_func));
    }
}
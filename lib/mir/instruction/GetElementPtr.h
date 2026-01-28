#pragma once

#include "FieldAccess.h"
#include "mir/value/ValueMatcher.h"

class GetElementPtr final: public FieldAccess {
public:
    explicit GetElementPtr(const NonTrivialType* basic_type, const Value &pointer, const Value &index) noexcept:
        FieldAccess({pointer, index}),
        m_basic_type(basic_type) {}

    [[nodiscard]]
    const Value &index() const {
        return m_values[1];
    }

    [[nodiscard]]
    const NonTrivialType* access_type() const noexcept override {
        return m_basic_type;
    }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<GetElementPtr> gep(const NonTrivialType* basic_type, const Value &pointer, const Value &index) {
        return std::make_unique<GetElementPtr>(basic_type, pointer, index);
    }

private:
    const NonTrivialType* m_basic_type;
};

namespace impls {
    template<typename T, typename ...Args>
    bool value_inst_with_operands(const Value& inst, Args&& ...args) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }
        const auto val = inst.get<ValueInstruction*>();
        if (const auto *typed = dynamic_cast<const T*>(val)) {
            return match_args(typed->operands(), std::forward<Args>(args)...);
        }

        return false;
    }
}

consteval auto gep() noexcept {
    return impls::value_inst<GetElementPtr>;
}

template<typename Matcher1, typename Matcher2>
consteval auto gep(Matcher1&& pointer, Matcher2&& index) noexcept {
    return [=](const Value& inst) {
        return impls::value_inst_with_operands<GetElementPtr>(inst, pointer, index);
    };
}
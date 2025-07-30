#pragma once

#include <string>
#include <vector>
#include <iosfwd>
#include <span>

#include "mir/mir_frwd.h"

class FunctionPrototype final {
public:
    explicit FunctionPrototype(const Type* ret_type, std::vector<const NonTrivialType*> arg_types, std::string name)
        : m_ret_type(ret_type), m_arg_types(std::move(arg_types)), m_name(std::move(name)) {}

    [[nodiscard]]
    const Type* ret_type() const { return m_ret_type; }

    [[nodiscard]]
    std::span<const NonTrivialType* const> arg_types() const {
        return m_arg_types;
    }

    const NonTrivialType* arg_type(const std::size_t index) const noexcept {
        return m_arg_types[index];
    }

    [[nodiscard]]
    std::string_view name() const { return m_name; }

    void print(std::ostream &os) const;
    void print(std::ostream &os, std::span<const ArgumentValue> args) const;
    void print(std::ostream &os, std::span<const Value> args) const;

private:
    const Type* m_ret_type;
    std::vector<const NonTrivialType*> m_arg_types;
    std::string m_name;
};
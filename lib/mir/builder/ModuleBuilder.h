#pragma once

#include <deque>
#include <expected>
#include <string>
#include <unordered_map>

#include "mir/mir_frwd.h"
#include "mir/global/GValuePool.h"
#include "mir/module/FunctionPrototypeTable.h"
#include "mir/module/Module.h"


class ModuleBuilder final {
public:
    [[nodiscard]]
    const FunctionPrototype* add_function_prototype(const Type* ret_type, std::vector<const NonTrivialType*>&& arg_types, std::string&& name, std::vector<AttributeSet>&& attributes, FunctionBind visibility);

    [[nodiscard]]
    const FunctionPrototype* add_function_prototype(const Type* ret_type, std::vector<const NonTrivialType*>&& arg_types, std::string&& name, FunctionBind visibility);

    [[nodiscard]]
    std::expected<FunctionBuilder, Error> make_function_builder(const FunctionPrototype *prototype);

    [[nodiscard]]
    const StructType* add_struct_type(std::string_view name, std::vector<const NonTrivialType*>&& field_types);

    [[nodiscard]]
    const ArrayType* add_array_type(const NonTrivialType* element_type, std::size_t length);

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_constant(const std::string_view name, const NonTrivialType* type, Initializer&& value) {
        return m_gvalue_pool.add_constant(name, type, std::move(value));
    }

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_variable(const std::string_view name, const NonTrivialType* type, Initializer&& value) {
        return m_gvalue_pool.add_variable(name, type, std::move(value));
    }

    [[nodiscard]]
    Module build() noexcept;

private:
    FunctionPrototypeTable m_prototypes;
    std::unordered_map<std::string, StructType> m_known_structs;
    std::deque<ArrayType> m_array_types;
    std::unordered_map<std::string, FunctionData> m_functions;
    GValuePool m_gvalue_pool;
};
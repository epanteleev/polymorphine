#pragma once

#include <deque>
#include <expected>
#include <string>
#include <unordered_map>

#include "mir/mir_frwd.h"
#include "mir/global/ConstantPool.h"
#include "mir/module/FunctionPrototypeTable.h"
#include "mir/module/Module.h"


class ModuleBuilder final {
public:
    const FunctionPrototype* add_function_prototype(const Type* ret_type, std::vector<const NonTrivialType*>&& arg_types, std::string&& name, std::vector<AttributeSet>&& attributes, FunctionLinkage linkage);
    const FunctionPrototype* add_function_prototype(const Type* ret_type, std::vector<const NonTrivialType*>&& arg_types, std::string&& name, FunctionLinkage linkage);

    std::expected<FunctionBuilder, Error> make_function_builder(const FunctionPrototype *prototype);

    const StructType* add_struct_type(std::string_view name, std::vector<const NonTrivialType*>&& field_types);

    const ArrayType* add_array_type(const NonTrivialType* element_type, std::size_t length);

    template <GlobalConstantVarians T>
    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> add_constant(std::string&& name, const NonTrivialType* type, T value) {
        return m_constant_pool.add_constant(std::move(name), type, value);
    }

    Module build() noexcept;

private:
    FunctionPrototypeTable m_prototypes;
    std::unordered_map<std::string, StructType> m_known_structs;
    std::deque<ArrayType> m_array_types;
    std::unordered_map<std::string, FunctionData> m_functions;
    ConstantPool m_constant_pool;
};
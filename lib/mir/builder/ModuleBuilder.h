#pragma once

#include <expected>
#include <string>
#include <unordered_map>

#include "mir/mir_frwd.h"
#include "mir/module/Module.h"


class ModuleBuilder final {
public:
    std::expected<FunctionBuilder*, Error> make_function_builder(FunctionPrototype&& prototype);

    Module build() noexcept;

    const StructType* add_struct_type(std::string_view name, std::vector<const NonTrivialType*>&& field_types);

    const ArrayType* add_array_type(const NonTrivialType* element_type, std::size_t length);

private:
    std::unordered_map<std::string, std::unique_ptr<StructType>> m_known_structs;
    std::vector<std::unique_ptr<ArrayType>> m_array_types;
    std::unordered_map<std::string, FunctionBuilder> m_functions;
};
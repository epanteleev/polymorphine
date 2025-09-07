#include "ModuleBuilder.h"
#include "FunctionBuilder.h"
#include "utility/Error.h"
#include "mir/types/ArrayType.h"

#include <ranges>

const FunctionPrototype * ModuleBuilder::add_function_prototype(const Type *ret_type, std::vector<const NonTrivialType *> &&arg_types, std::string &&name, std::vector<AttributeSet> &&attributes, const FunctionLinkage linkage) {
    return m_prototypes.insert(ret_type, std::move(arg_types), std::move(name), std::move(attributes), linkage).first;
}

const FunctionPrototype * ModuleBuilder::add_function_prototype(const Type *ret_type, std::vector<const NonTrivialType *> &&arg_types, std::string &&name, const FunctionLinkage linkage) {
    return m_prototypes.insert(ret_type, std::move(arg_types), std::move(name), std::vector(arg_types.size(), AttributeSet{}), linkage).first;
}

std::expected<FunctionBuilder*, Error> ModuleBuilder::make_function_builder(const FunctionPrototype *prototype) {
    std::string str(prototype->name());

    std::vector<ArgumentValue> args;
    args.reserve(prototype->arg_types().size());
    for (auto [idx, tp]: std::ranges::views::enumerate(prototype->arg_types())) {
        args.emplace_back(idx, tp, prototype->attribute(idx));
    }

    auto [b, inserted] = m_functions.emplace(str, FunctionBuilder::make(prototype, std::move(args)));
    if (!inserted) {
        return std::unexpected(Error::CastError);
    }

    return &b->second;
}

Module ModuleBuilder::build() noexcept {
    std::unordered_map<std::string, std::unique_ptr<FunctionData>> functions;
    functions.reserve(m_functions.size());
    for (auto&& [n, b] : m_functions) {
        functions[n] = b.build();
    }

    return Module(std::move(m_prototypes), std::move(functions), std::move(m_known_structs), std::move(m_array_types));
}

const StructType *ModuleBuilder::add_struct_type(const std::string_view name, std::vector<const NonTrivialType *> &&field_types) {
    const auto& [it, inserted] = m_known_structs.emplace(std::string(name), StructType::make(name, std::move(field_types)));
    assertion(inserted, "Struct type {} already defined", name);
    return &it->second;
}

const ArrayType *ModuleBuilder::add_array_type(const NonTrivialType *element_type, const std::size_t length) {
    m_array_types.push_back(ArrayType::make(element_type, length));
    return &m_array_types.back();
}

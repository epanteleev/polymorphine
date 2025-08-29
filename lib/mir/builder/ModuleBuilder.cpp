#include "ModuleBuilder.h"
#include "FunctionBuilder.h"
#include "utility/Error.h"

#include <ranges>

std::expected<FunctionBuilder*, Error> ModuleBuilder::make_function_builder(FunctionPrototype &&prototype) {
    std::string str(prototype.name());

    std::vector<ArgumentValue> args;
    args.reserve(prototype.arg_types().size());
    for (auto [idx, tp]: std::ranges::views::enumerate(prototype.arg_types())) {
        args.emplace_back(idx, tp);
    }

    auto [b, inserted] = m_functions.emplace(str, FunctionBuilder::make(std::move(prototype), std::move(args)));
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

    return Module(std::move(functions), std::move(m_known_structs));
}

const StructType * ModuleBuilder::add_struct_type(std::string_view name, std::vector<const NonTrivialType *> &&field_types) {
    const auto& [it, inserted] = m_known_structs.emplace(std::string(name), StructType::make(name, std::move(field_types)));
    assertion(inserted, "Struct type {} already defined", name);
    return it->second.get();
}

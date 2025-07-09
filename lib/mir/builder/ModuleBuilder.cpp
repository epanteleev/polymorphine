#include "ModuleBuilder.h"
#include "FunctionBuilder.h"

#include <ranges>

#include "utility/Error.h"

std::expected<FunctionBuilder*, Error> ModuleBuilder::make_function_builder(FunctionPrototype &&prototype) {
    auto str = std::string(prototype.name());

    std::vector<ArgumentValue> args;
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
    for (auto&& [n, b] : m_functions) {
        functions[n] = b.build();
    }

    return Module(std::move(functions));
}

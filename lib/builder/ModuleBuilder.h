#pragma once

#include <string>
#include <unordered_map>

#include "FunctionBuilder.h"
#include "module/FunctionData.h"
#include "module/Module.h"


class ModuleBuilder final {
public:
    FunctionBuilder* make_function_builder(FunctionPrototype&& prototype, std::vector<ArgumentValue>&& args) {
        auto str = std::string(prototype.name());
        auto [b, _] = m_functions.emplace(str, FunctionBuilder::make(std::move(prototype), std::move(args)));
        return &b->second;
    }

    Module build() noexcept;

private:
    std::unordered_map<std::string, FunctionBuilder> m_functions;
};
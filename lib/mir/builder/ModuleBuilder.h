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

private:
    std::unordered_map<std::string, FunctionBuilder> m_functions;
};
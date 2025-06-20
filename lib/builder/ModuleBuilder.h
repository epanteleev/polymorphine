#pragma once

#include <string>
#include <unordered_map>

#include "ir_frwd.h"
#include "module/Module.h"


class ModuleBuilder final {
public:
    std::expected<FunctionBuilder*, Error> make_function_builder(FunctionPrototype&& prototype);

    Module build() noexcept;

private:
    std::unordered_map<std::string, FunctionBuilder> m_functions;
};
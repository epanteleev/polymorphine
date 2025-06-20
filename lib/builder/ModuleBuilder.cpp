#include "ModuleBuilder.h"

Module ModuleBuilder::build() noexcept {
    std::unordered_map<std::string, std::unique_ptr<FunctionData>> functions;
    for (auto&& [n, b] : m_functions) {
        functions[n] = b.build();
    }

    return Module(std::move(functions));
}

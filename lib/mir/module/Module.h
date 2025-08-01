#pragma once

#include <iosfwd>

#include "mir/module/FunctionData.h"

class Module final {
public:
    explicit Module(std::unordered_map<std::string, std::unique_ptr<FunctionData>>&& functions) noexcept:
        m_functions(std::move(functions)) {}

    std::optional<FunctionData*> find_function_data(const std::string& name) {
        const auto& it = m_functions.find(name);
        if (it == m_functions.end()) {
            return std::nullopt;
        }

        return it->second.get();
    }

    const std::unordered_map<std::string, std::unique_ptr<FunctionData>>& functions() const {
        return m_functions;
    }

    friend std::ostream &operator<<(std::ostream &os, const Module &module);

private:
    std::unordered_map<std::string, std::unique_ptr<FunctionData>> m_functions;
};

std::ostream & operator<<(std::ostream &os, const Module &module);
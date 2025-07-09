#pragma once

#include <iosfwd>

#include "mir/module/FunctionData.h"

class Module final {
public:
    explicit Module(std::unordered_map<std::string, std::unique_ptr<FunctionData>>&& functions) noexcept:
        m_functions(std::move(functions)) {}

    const FunctionData* add_function_data(FunctionPrototype&& proto, std::vector<ArgumentValue>&& args) {
        const auto& [fst, snd] = m_functions.emplace(proto.name(), std::make_unique<FunctionData>(std::move(proto), std::move(args)));
        return fst->second.get();
    }

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

    std::ostream &print(std::ostream &os) const;

private:
    std::unordered_map<std::string, std::unique_ptr<FunctionData>> m_functions;
};

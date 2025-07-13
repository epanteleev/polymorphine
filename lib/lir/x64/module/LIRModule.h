#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <ranges>

#include "LIRFuncData.h"


class LIRModule final {
public:
    explicit LIRModule(std::unordered_map<std::string, std::unique_ptr<LIRFuncData>>&& functions) noexcept:
        m_functions(std::move(functions)) {}

    std::expected<LIRFuncData*, Error> add_function_data(const std::string& proto, std::vector<LIRArg>&& args) {
        auto name = std::string(proto);
        const auto& [fst, snd] = m_functions.emplace(proto, std::make_unique<LIRFuncData>(std::move(name), std::move(args)));
        if (!snd) {
            return std::unexpected(Error::FunctionExistsError);
        }

        return fst->second.get();
    }

    std::expected<LIRFuncData*, Error> find_function_data(const std::string& name) const {
        const auto& it = m_functions.find(name);
        if (it == m_functions.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return it->second.get();
    }

    decltype(auto) functions() const noexcept {
        return m_functions | std::views::values;
    }

    [[nodiscard]]

    std::ostream &print(std::ostream &os) const;

private:
    std::unordered_map<std::string, std::unique_ptr<LIRFuncData>> m_functions;
};

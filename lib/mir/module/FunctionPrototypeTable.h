#pragma once
#include <expected>
#include <memory>
#include <string>
#include <unordered_map>

#include "FunctionPrototype.h"
#include "utility/Error.h"

class FunctionPrototypeTable final {
public:
    [[nodiscard]]
    std::expected<const FunctionPrototype*, Error> lookup(const std::string& name) const {
        const auto it = m_table.find(name);
        if (it == m_table.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return &it->second;
    }

    std::pair<const FunctionPrototype*, bool> insert(const Type* ret_type, std::vector<const NonTrivialType*>&& arg_types, std::string&& name, std::vector<AttributeSet>&& attributes, const FunctionVisibility linkage) {
        auto key = name;
        const auto [it, inserted] = m_table.try_emplace(std::move(key), ret_type, std::move(arg_types), std::move(name), std::move(attributes), linkage);
        if (!inserted) {
            return {&it->second, false};
        }

        return {&it->second, true};
    }

    auto begin() const noexcept {
        return m_table.begin();
    }

    auto end() const noexcept {
        return m_table.end();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_table.empty();
    }

private:
    std::unordered_map<std::string, FunctionPrototype> m_table;
};
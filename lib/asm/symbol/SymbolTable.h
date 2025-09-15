#pragma once

#include <deque>
#include <optional>
#include <unordered_map>

#include "Symbol.h"

namespace aasm {
    /**
     * Represents a symbol in the assembly code, which can be a label or a function name.
     */
    class SymbolTable final {
    public:
        std::pair<const Symbol*, bool> add(const std::string_view name, const BindAttribute bind) {
            if (const auto it = m_symbol_map.find(std::string(name)); it != m_symbol_map.end()) { //TODO string creation
                return {it->second, false};
            }

            const auto& new_symbol = m_symbols.emplace_back(name.data(), bind);
            m_symbol_map.emplace(name, &new_symbol);
            return {&new_symbol, true};
        }

        std::optional<const Symbol*> find(const std::string& name) const {
            if (const auto it = m_symbol_map.find(name); it != m_symbol_map.end()) {
                return it->second;
            }

            return std::nullopt;
        }

    private:
        std::deque<Symbol> m_symbols;
        std::unordered_map<std::string, const Symbol*> m_symbol_map;
    };
}
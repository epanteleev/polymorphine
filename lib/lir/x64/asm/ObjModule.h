#pragma once

#include <expected>
#include <unordered_map>
#include <iomanip>

#include "AsmEmitter.h"

class ObjModule final {
public:
    explicit ObjModule(std::unordered_map<std::string, aasm::AsmBuffer> &&modules) noexcept
        : m_modules(std::move(modules)) {}

    std::expected<aasm::AsmBuffer*, Error> find_masm(const std::string &name) {
        if (const auto it = m_modules.find(name); it != m_modules.end()) {
            return &it->second;
        }

        return std::unexpected(Error::NotFoundError);
    }

    const auto& emitters() const noexcept {
        return m_modules;
    }

    friend std::ostream& operator<<(std::ostream &os, const ObjModule &module);

private:
    std::unordered_map<std::string, aasm::AsmBuffer> m_modules;
};

inline std::ostream & operator<<(std::ostream &os, const ObjModule &module) {
    for (const auto& [name, masm]: module.m_modules) {
        os << name << ':' << std::endl;
        os << std::setw(4) << masm << std::endl;
    }

    return os;
}

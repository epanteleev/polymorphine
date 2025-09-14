#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "LIRFuncData.h"
#include "lir/x64/global/GlobalData.h"

class LIRModule final {
public:
    using const_iterator = std::unordered_map<std::string, LIRFuncData>::const_iterator;
    using iterator = std::unordered_map<std::string, LIRFuncData>::iterator;

    explicit LIRModule(std::unordered_map<std::string, LIRFuncData>&& functions, GlobalData&& global_data) noexcept:
        m_functions(std::move(functions)),
        m_global_data(std::move(global_data)) {}

    std::expected<LIRFuncData*, Error> find_function_data(const std::string& name) {
        const auto& it = m_functions.find(name);
        if (it == m_functions.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return &it->second;
    }

    iterator begin() noexcept {
        return m_functions.begin();
    }

    iterator end() noexcept {
        return m_functions.end();
    }

    const_iterator begin() const noexcept {
        return m_functions.begin();
    }

    const_iterator end() const noexcept {
        return m_functions.end();
    }

    friend std::ostream& operator<<(std::ostream &os, const LIRModule &module);

private:
    std::unordered_map<std::string, LIRFuncData> m_functions;
    GlobalData m_global_data;
};

std::ostream & operator<<(std::ostream &os, const LIRModule &module);
#pragma once

#include <iosfwd>

#include "mir/module/FunctionData.h"
#include "mir/types/StructType.h"


class Module final {
public:
    explicit Module(std::unordered_map<std::string, std::unique_ptr<FunctionData>>&& functions, std::unordered_map<std::string, std::unique_ptr<StructType>>&& known_structs) noexcept:
        m_known_structs(std::move(known_structs)),
        m_functions(std::move(functions)) {}

    const FunctionData* add_function_data(FunctionPrototype&& proto, std::vector<ArgumentValue>&& args);

    std::expected<FunctionData*, Error> find_function_data(const std::string& name) {
        const auto& it = m_functions.find(name);
        if (it == m_functions.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return it->second.get();
    }

    const std::unordered_map<std::string, std::unique_ptr<FunctionData>>& functions() const {
        return m_functions;
    }

    friend std::ostream &operator<<(std::ostream &os, const Module &module);

private:
    std::unordered_map<std::string, std::unique_ptr<StructType>> m_known_structs;
    std::unordered_map<std::string, std::unique_ptr<FunctionData>> m_functions;
};

std::ostream & operator<<(std::ostream &os, const Module &module);
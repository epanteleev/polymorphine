#pragma once

#include <deque>
#include <iosfwd>

#include "FunctionPrototypeTable.h"
#include "mir/global/ConstantPool.h"
#include "mir/module/FunctionData.h"
#include "mir/types/StructType.h"
#include "mir/types/ArrayType.h"


class Module final {
public:
    explicit Module(FunctionPrototypeTable&& prototypes,
        std::unordered_map<std::string, FunctionData>&& functions,
        std::unordered_map<std::string, StructType>&& known_structs,
        std::deque<ArrayType>&& array_types,
        ConstantPool&& constant_pool) noexcept:
        m_prototypes(std::move(prototypes)),
        m_known_structs(std::move(known_structs)),
        m_array_types(std::move(array_types)),
        m_functions(std::move(functions)),
        m_constant_pool(std::move(constant_pool)) {}

    const FunctionData* add_function_data(const FunctionPrototype* proto, std::vector<ArgumentValue>&& args);

    std::expected<FunctionData*, Error> find_function_data(const std::string& name) {
        const auto& it = m_functions.find(name);
        if (it == m_functions.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return &it->second;
    }

    const std::unordered_map<std::string, FunctionData>& functions() const {
        return m_functions;
    }

    [[nodiscard]]
    const ConstantPool& constant_pool() const noexcept {
        return m_constant_pool;
    }

    friend std::ostream &operator<<(std::ostream &os, const Module &module);

private:
    FunctionPrototypeTable m_prototypes;
    std::unordered_map<std::string, StructType> m_known_structs;
    std::deque<ArrayType> m_array_types;
    std::unordered_map<std::string, FunctionData> m_functions;
    ConstantPool m_constant_pool;
};

std::ostream & operator<<(std::ostream &os, const Module &module);
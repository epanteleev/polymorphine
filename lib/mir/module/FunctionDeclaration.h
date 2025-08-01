#pragma once

#include <cstdint>

#include "FunctionPrototype.h"

enum class FunctionLinkage: std::uint8_t {
    EXTERN,
    INTERNAL,
};

std::ostream & operator<<(std::ostream &os, const FunctionLinkage &linkage);

class FunctionDeclaration final {
public:
    explicit FunctionDeclaration(const FunctionLinkage linkage, FunctionPrototype&& prototype) noexcept:
        m_linkage(linkage),
        m_prototype(std::move(prototype)) {}

    explicit FunctionDeclaration(const FunctionLinkage linkage, const Type* ret_type, std::vector<const NonTrivialType*> arg_types, std::string name) noexcept:
        m_linkage(linkage),
        m_prototype(ret_type, std::move(arg_types), std::move(name)) {}

    [[nodiscard]]
    const FunctionPrototype& prototype() const noexcept {
        return m_prototype;
    }

    [[nodiscard]]
    const Type* ret_type() const { return m_prototype.ret_type(); }

    [[nodiscard]]
    std::string_view name() const noexcept { return m_prototype.name(); }

    friend std::ostream &operator<<(std::ostream &os, const FunctionDeclaration &decl);

private:
    FunctionLinkage m_linkage;
    FunctionPrototype m_prototype;
};

std::ostream & operator<<(std::ostream &os, const FunctionDeclaration &decl);
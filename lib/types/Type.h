#pragma once

#include <cstddef>
#include <functional>

#include "TypeMatcher.h"

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]]
    bool isa(const TypeMatcher &matcher) const {
        return matcher(this);
    }
};

class NonTrivialType : public Type {
public:
    [[nodiscard]]
    virtual std::size_t size_of() const = 0;

    [[nodiscard]]
    virtual std::size_t align_of() const = 0;
};

class PrimitiveType : public NonTrivialType {
public:
    [[nodiscard]]
    std::size_t align_of() const final {
        return size_of();
    }
};

class IntegerType : public PrimitiveType {};

class UnsignedIntegerType final: public IntegerType {
public:
    explicit UnsignedIntegerType(std::size_t size) : m_size(size) {}

    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

private:
    std::size_t m_size;
};

class SignedIntegerType final: public IntegerType {
public:
    explicit SignedIntegerType(const std::size_t size) : m_size(size) {}

    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

private:
    const std::size_t m_size;
};

class FloatingPointType final : public PrimitiveType {
public:
    explicit FloatingPointType(const std::size_t size)
        : m_size(size) {}

    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

private:
    const std::size_t m_size;
};

template<typename T>
concept IsType = std::is_base_of_v<T, Type> ||
                 std::is_base_of_v<T, NonTrivialType> ||
                 std::is_base_of_v<T, PrimitiveType> ||
                 std::is_base_of_v<T, IntegerType> ||
                 std::is_base_of_v<T, UnsignedIntegerType> ||
                 std::is_base_of_v<T, SignedIntegerType> ||
                 std::is_base_of_v<T, FloatingPointType>;

static_assert(IsType<Type>);
static_assert(IsType<NonTrivialType>);
static_assert(IsType<PrimitiveType>);
static_assert(IsType<IntegerType>);
static_assert(IsType<UnsignedIntegerType>);
static_assert(IsType<SignedIntegerType>);
static_assert(IsType<FloatingPointType>);
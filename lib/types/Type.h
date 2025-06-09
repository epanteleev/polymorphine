#pragma once

#include <cstddef>
#include <functional>
#include <iosfwd>

#include "TypeVisitor.h"
#include "TypeMatcher.h"

class Type {
public:
    virtual ~Type() = default;

    [[nodiscard]]
    bool isa(const TypeMatcher &matcher) const {
        return matcher(this);
    }

    void print(std::ostream &os) const;

    virtual void visit(type::Visitor &visitor) = 0;
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

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

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

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static SignedIntegerType* i8() noexcept;
    static SignedIntegerType* i16() noexcept;
    static SignedIntegerType* i32() noexcept;
    static SignedIntegerType* i64() noexcept;

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

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

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
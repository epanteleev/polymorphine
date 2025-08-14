#pragma once

#include <cstddef>
#include <iosfwd>

#include "TypeVisitor.h"

class Type {
public:
    virtual ~Type() = default;

    template<typename Matcher>
    [[nodiscard]]
    bool isa(const Matcher &matcher) const noexcept {
        return matcher(this);
    }

    friend std::ostream& operator<<(std::ostream &os, const Type &obj);

    bool operator==(const Type &other) const {
        return this == &other;
    }

    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

    virtual void visit(type::Visitor &visitor) = 0;
};

std::ostream& operator<<(std::ostream &os, const Type &obj);

class VoidType final: public Type {
    constexpr VoidType() = default;

public:
    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const VoidType *type() noexcept {
        static constexpr VoidType instance;
        return &instance;
    }
};

class FlagType final: public Type {
    FlagType() = default;
public:

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const FlagType *flag() noexcept {
        static constexpr FlagType flag_instance;
        return &flag_instance;
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
    constexpr explicit UnsignedIntegerType(const std::size_t size) : m_size(size) {}

public:
    [[nodiscard]]
    constexpr std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const UnsignedIntegerType * u8() noexcept {
        static constexpr UnsignedIntegerType u8_instance(1);
        return &u8_instance;
    }

    static consteval const UnsignedIntegerType *u16() noexcept {
        static constexpr UnsignedIntegerType u16_instance(2);
        return &u16_instance;
    }

    static consteval const UnsignedIntegerType *u32() noexcept {
        static constexpr UnsignedIntegerType u32_instance(4);
        return &u32_instance;
    }

    static consteval const UnsignedIntegerType *u64() noexcept {
        static constexpr UnsignedIntegerType u64_instance(8);
        return &u64_instance;
    }

private:
    std::size_t m_size;
};

class SignedIntegerType final: public IntegerType {
    explicit constexpr SignedIntegerType(const std::size_t size) : m_size(size) {}

public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const SignedIntegerType * i8() noexcept {
        static constexpr SignedIntegerType i8_instance(1);
        return &i8_instance;
    }

    static consteval const SignedIntegerType *i16() noexcept {
        static constexpr SignedIntegerType i16_instance(2);
        return &i16_instance;
    }

    static consteval const SignedIntegerType *i32() noexcept {
        static constexpr SignedIntegerType i32_instance(4);
        return &i32_instance;
    }

    static consteval const SignedIntegerType *i64() noexcept {
        static constexpr SignedIntegerType i8_instance(8);
        return &i8_instance;
    }

private:
    const std::size_t m_size;
};

class FloatingPointType final : public PrimitiveType {
    explicit FloatingPointType(const std::size_t size)
        : m_size(size) {}

public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

private:
    const std::size_t m_size;
};

class PointerType final : public PrimitiveType {
public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return 8;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const PointerType * ptr() noexcept {
        static constexpr PointerType p;
        return &p;
    }
};
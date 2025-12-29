#pragma once

#include "ArithmeticType.h"

class IntegerType : public ArithmeticType {};

class UnsignedIntegerType final: public IntegerType {
    constexpr explicit UnsignedIntegerType(const std::size_t size) noexcept:
        m_size(size) {}

public:
    [[nodiscard]]
    constexpr std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const UnsignedIntegerType *u8() noexcept {
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
    explicit constexpr SignedIntegerType(const std::size_t size) noexcept:
        m_size(size) {}

public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const SignedIntegerType *i8() noexcept {
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
        static constexpr SignedIntegerType i64_instance(8);
        return &i64_instance;
    }

private:
    const std::size_t m_size;
};
#pragma once

#include "ArithmeticType.h"

class IntegerType : public ArithmeticType {
public:
    static constexpr const IntegerType *cast(const Type* ty) noexcept;
};

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

    [[nodiscard]]
    static constexpr const UnsignedIntegerType *cast(const Type* ty) {
        static constexpr const UnsignedIntegerType* uint_types[] = { u8(), u16(), u32(), u64() };
        for (const auto& type : uint_types) {
            if (type == ty) return type;
        }

        return nullptr;
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

    [[nodiscard]]
    static constexpr const SignedIntegerType *cast(const Type* ty) {
        static constexpr const SignedIntegerType* int_types[] = { i8(), i16(), i32(), i64() };
        for (const auto& type : int_types) {
            if (type == ty) return type;
        }

        return nullptr;
    }
private:
    const std::size_t m_size;
};

constexpr const IntegerType * IntegerType::cast(const Type *ty) noexcept {
    static constexpr const IntegerType* int_types[] = {
        SignedIntegerType::i8(),
        SignedIntegerType::i16(),
        SignedIntegerType::i32(),
        SignedIntegerType::i64(),
        UnsignedIntegerType::u8(),
        UnsignedIntegerType::u16(),
        UnsignedIntegerType::u32(),
        UnsignedIntegerType::u64()
    };
    for (const auto& type : int_types) {
        if (type == ty) return type;
    }

    return nullptr;
}

namespace impls {
    inline bool signed_type(const Type *type) noexcept {
        return SignedIntegerType::cast(type) != nullptr;
    }

    inline bool unsigned_type(const Type *type) noexcept {
        return UnsignedIntegerType::cast(type) != nullptr;
    }

    template<typename T, std::size_t SIZE>
    bool is_type_with_size(const Type *type) noexcept {
        const auto as_type = T::cast(type);
        if (!as_type) {
            return false;
        }

        return as_type->size_of() == SIZE;
    }
}

consteval auto signed_type() noexcept {
    return impls::signed_type;
}

consteval auto unsigned_type() noexcept {
    return impls::unsigned_type;
}


consteval auto i8() noexcept {
    return impls::is_type_with_size<SignedIntegerType, 1>;
}

consteval auto u8() noexcept {
    return impls::is_type_with_size<UnsignedIntegerType, 1>;
}

consteval auto byte_type() noexcept {
    return impls::is_type_with_size<IntegerType, 1>;
}

consteval auto i16() noexcept {
    return impls::is_type_with_size<SignedIntegerType, 2>;
}

consteval auto u16() noexcept {
    return impls::is_type_with_size<UnsignedIntegerType, 2>;
}

consteval auto i32() noexcept {
    return impls::is_type_with_size<SignedIntegerType, 4>;
}

consteval auto u32() noexcept {
    return impls::is_type_with_size<UnsignedIntegerType, 4>;
}

consteval auto i64() noexcept {
    return impls::is_type_with_size<SignedIntegerType, 8>;
}

consteval auto u64() noexcept {
    return impls::is_type_with_size<UnsignedIntegerType, 8>;
}
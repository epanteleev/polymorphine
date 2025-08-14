#pragma once

namespace impls {
    inline bool signed_type(const Type *type) noexcept {
        return dynamic_cast<const SignedIntegerType *>(type) != nullptr;
    }

    inline bool unsigned_type(const Type *type) noexcept {
        return dynamic_cast<const UnsignedIntegerType *>(type) != nullptr;
    }

    inline bool primitive(const Type *type) noexcept {
        return dynamic_cast<const PrimitiveType *>(type) != nullptr;
    }

    template<typename T, std::size_t SIZE>
    bool is_signed_with_size(const Type *type) noexcept {
        const auto as_signed = dynamic_cast<const T *>(type);
        if (!as_signed) {
            return false;
        }

        return as_signed->size_of() == SIZE;
    }

    inline bool void_type(const Type *type) noexcept {
        return dynamic_cast<const VoidType *>(type) != nullptr;
    }
}

consteval auto signed_type() noexcept {
    return impls::signed_type;
}

consteval auto unsigned_type() noexcept {
    return impls::unsigned_type;
}

consteval auto primitive() noexcept {
    return impls::primitive;
}

consteval auto i8() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 1>;
}

consteval auto u8() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 1>;
}

consteval auto i16() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 2>;
}

consteval auto u16() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 2>;
}

consteval auto i32() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 4>;
}

consteval auto u32() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 4>;
}

consteval auto i64() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 8>;
}

consteval auto u64() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 8>;
}

consteval auto void_type() noexcept {
    return impls::void_type;
}
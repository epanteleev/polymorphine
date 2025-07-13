#pragma once

namespace impls {
    inline bool signedType(const Type *type) {
        return dynamic_cast<const SignedIntegerType *>(type) != nullptr;
    }

    template<typename T, std::size_t SIZE>
    bool is_signed_with_size(const Type *type) noexcept {
        const auto as_signed = dynamic_cast<const T *>(type);
        if (!as_signed) {
            return false;
        }

        return as_signed->size_of() == SIZE;
    }
}

constexpr auto signedType() {
    return impls::signedType;
}

constexpr auto i8() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 1>;
}

constexpr auto u8() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 1>;
}

constexpr auto i16() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 2>;
}

constexpr auto u16() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 2>;
}

constexpr auto i32() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 4>;
}

constexpr auto u32() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 4>;
}

constexpr auto i64() noexcept {
    return impls::is_signed_with_size<SignedIntegerType, 8>;
}

constexpr auto u64() noexcept {
    return impls::is_signed_with_size<UnsignedIntegerType, 8>;
}

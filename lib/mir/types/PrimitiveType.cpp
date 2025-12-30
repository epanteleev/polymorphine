#include "PrimitiveType.h"

#include "IntegerType.h"
#include "FloatingPointType.h"
#include "PointerType.h"

const PrimitiveType* PrimitiveType::cast(const Type *ty) noexcept {
    static constexpr const PrimitiveType* primitives[] = {
        SignedIntegerType::i8(),
        SignedIntegerType::i16(),
        SignedIntegerType::i32(),
        SignedIntegerType::i64(),
        UnsignedIntegerType::u8(),
        UnsignedIntegerType::u16(),
        UnsignedIntegerType::u32(),
        UnsignedIntegerType::u64(),
        FloatingPointType::f32(),
        FloatingPointType::f64(),
        PointerType::ptr()
    };
    for (const auto& type : primitives) {
        if (type == ty) return type;
    }

    return nullptr;
}

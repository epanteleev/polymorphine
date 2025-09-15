#include "ConstantPool.h"

#include "mir/types/ArrayType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/TypeMatcher.h"

std::expected<const GlobalConstant *, Error> ConstantPool::add_constant(const std::string_view name, const ArrayType *type, const std::string_view value) {
#ifndef NDEBUG
    if (!type->element_type()->isa(byte_type())) {
        return std::unexpected(Error::InvalidArgument);
    }
    if (type->length() == 0 || type->length() < value.size()) {
        return std::unexpected(Error::InvalidArgument);
    }
#endif
    return add_constant_internal(name, type, value.data());
}
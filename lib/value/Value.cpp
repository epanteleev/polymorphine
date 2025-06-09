#include "Value.h"
#include "../types/Type.h"


Value::Value(double value, FloatingPointType *type): m_value(value),
                                                     m_type(type) {
}

Value::Value(std::uint64_t value, UnsignedIntegerType *type): m_value(value),
                                                                m_type(type) {
}

Value::Value(std::int64_t value, SignedIntegerType *type): m_value(value),
                                                      m_type(type) {
}
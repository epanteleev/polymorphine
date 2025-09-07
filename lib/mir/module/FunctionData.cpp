#include "FunctionData.h"

FunctionData::FunctionData(const FunctionPrototype* proto, std::vector<ArgumentValue> &&args) noexcept:
    FunctionDataBase(std::move(args)),
    m_prototype(proto) {
    create_basic_block();
}
#include "FunctionData.h"

FunctionData::FunctionData(FunctionPrototype &&proto, std::vector<ArgumentValue> &&args) noexcept:
    FunctionDataBase(std::move(args)),
    m_prototype(std::move(proto)) {
    create_basic_block();
}
#include "FunctionData.h"

FunctionData::FunctionData(const FunctionPrototype* prototype, std::vector<ArgumentValue> &&args) noexcept:
    FunctionDataBase(std::move(args)),
    m_prototype(prototype) {
    create_basic_block();
}

std::ostream & operator<<(std::ostream &os, const FunctionData &fd) {
    os << "define ";
    fd.m_prototype->print(os, fd.m_args);
    os << ' ';
    fd.print_blocks(os);
    return os;
}

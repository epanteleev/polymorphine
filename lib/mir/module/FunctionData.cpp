#include "FunctionData.h"

FunctionData::FunctionData(const std::size_t uid, const FunctionPrototype* prototype, std::vector<ArgumentValue> &&args) noexcept:
    FunctionDataBase(uid, std::move(args)),
    m_prototype(prototype) {
    create_basic_block();
}

static std::ostream& print_blocks(std::ostream &os, const OrderedSet<BasicBlock> &blocks) {
    os << '{' << std::endl;
    for (const auto &bb : blocks) {
        bb.print(os);
    }
    os << '}' << std::endl;
    return os;
}

std::ostream & operator<<(std::ostream &os, const FunctionData &fd) {
    os << "define ";
    fd.m_prototype->print(os, fd.m_args);
    os << ' ';
    print_blocks(os, fd.m_basic_blocks);
    return os;
}

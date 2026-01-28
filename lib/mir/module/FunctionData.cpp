#include "FunctionData.h"

#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"

FunctionData::FunctionData(const std::size_t uid, const FunctionPrototype* prototype, std::vector<ArgumentValue> &&args) noexcept:
    FunctionDataBase(uid, std::move(args)),
    m_prototype(prototype) {
    create_basic_block();
}

BasicBlock * FunctionData::last() const {
    const auto last_bb = m_basic_blocks.back();
    assertion(last_bb != m_basic_blocks.end(), "last basic block is null");
    assertion(last_bb->last().isa(any_return()), "last basic block is not a return block");
    return last_bb.get();
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

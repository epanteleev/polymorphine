
#include <iostream>
#include <ranges>

#include "FunctionData.h"
#include "utility/Error.h"
#include "instruction/TerminateInstruction.h"

FunctionData::FunctionData(FunctionPrototype &&proto, std::vector<ArgumentValue> &&args) :
    m_prototype(std::move(proto)),
    m_args(std::move(args)) {

    const auto begin = create_basic_block();

    assertion(begin != nullptr, "Failed to create basic block");
    assertion(begin->id() == 0, "First basic block should have id 0");

    assertion(m_args.size() == m_prototype.arg_types().size(),
              "Number of arguments does not match prototype m_args={}, arg_types={}", m_args.size(), m_prototype.arg_types().size());

#ifdef ENABLE_ASSERTIONS
    for (auto [a, b]: std::ranges::views::zip(m_args, proto.arg_types())) {
        assertion(a.type() == b, "Argument type mismatch");
    }
#endif
}

void FunctionData::print(std::ostream &os) const {
    os << "define ";
    m_prototype.print(os, m_args);
    os << " {";
    os << std::endl;
    for (const auto &bb : m_basic_blocks) {
        bb.print(os);
    }
    os << std::endl;
    os << "}" << std::endl;
}

BasicBlock * FunctionData::last() const {
    const auto last_bb = m_basic_blocks.back();

#ifdef ENABLE_ASSERTIONS
    assertion(last_bb.has_value(), "invariant");
    const auto last_inst = last_bb.value()->last();
    assertion(last_inst.as<Return>() || last_inst.as<ReturnValue>(), "invariant");
#endif

    const auto ret = last_bb.value();
    return ret;
}

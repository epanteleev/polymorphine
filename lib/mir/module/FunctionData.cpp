#include <ranges>

#include "FunctionData.h"
#include "utility/Error.h"
#include "mir/instruction/TerminateInstruction.h"

FunctionData::FunctionData(FunctionPrototype &&proto, std::vector<ArgumentValue> &&args) :
    FunctionDataBase(std::move(args)),
    m_prototype(std::move(proto)) {

    const auto begin = create_basic_block();

    assertion(begin != nullptr, "Failed to create basic block");
    assertion(begin->id() == 0, "First basic block should have id 0");

    assertion(m_args.size() == m_prototype.arg_types().size(),
              "Number of arguments does not match prototype m_args={}, arg_types={}", m_args.size(), m_prototype.arg_types().size());

#ifndef NDEBUG
    for (auto [a, b]: std::ranges::views::zip(m_args, proto.arg_types())) {
        assertion(a.type() == b, "Argument type mismatch");
    }
#endif
}
#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/instruction/LIRReturn.h"
#include "lir/x64/instruction/LIRAdjustStack.h"

#include <ostream>

LIRBlock * LIRFuncData::last() const {
    const auto last_bb = m_basic_blocks.back();
    assertion(last_bb != m_basic_blocks.end(), "last basic block is null");
    const auto ret = dynamic_cast<const LIRReturn*>(last_bb->last());
    assertion(ret != nullptr, "last instruction is not a return");
    return last_bb.get();
}

LIRAdjustStack * LIRFuncData::prologue() const {
    auto& first_instruction = first()->at(0);
    const auto prologue = dynamic_cast<LIRAdjustStack *>(&first_instruction);
    assertion(prologue != nullptr, "must be");
    return prologue;
}

LIRAdjustStack * LIRFuncData::epilogue() const {
    auto& last_instruction = last()->at(last()->size() - 2);
    const auto epilogue = dynamic_cast<LIRAdjustStack *>(&last_instruction);
    assertion(epilogue != nullptr, "must be");
    return epilogue;
}

static std::ostream& print_blocks(std::ostream &os, const OrderedSet<LIRBlock> &blocks) {
    os << '{' << std::endl;
    for (const auto &bb : blocks) {
        bb.print(os);
    }
    os << '}' << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const LIRFuncData &fd) {
    os << fd.m_name << '(';
    for (auto [idx, arg] : std::ranges::enumerate_view(fd.m_args)) {
        if (idx > 0) {
            os << ", ";
        }

        os << arg;
    }
    os << ") ";
    if (!fd.m_global_data.empty()) {
        os << std::endl << "constants: [" << std::endl;
        for (const auto& [idx, data] : std::ranges::enumerate_view(fd.m_global_data)) {
            if (idx != 0) {
                os << std::endl;
            }

            data.second.print_description(os);
        }
        os << "]" << std::endl;
    }
    print_blocks(os, fd.m_basic_blocks);
    return os;
}

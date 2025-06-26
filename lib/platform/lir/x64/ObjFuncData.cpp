#include "ObjFuncData.h"

#include <ostream>


ObjFuncData::ObjFuncData(std::string_view name, std::vector<LIRArg> &&args) noexcept
        : m_name(name), m_args(std::move(args)) {
    create_mach_block();
}

MachBlock * ObjFuncData::last() const {
    const auto last_bb = m_basic_blocks.back();
    return last_bb.value();
}


void ObjFuncData::print(std::ostream &os) const {
    os << m_name << '(';
    for (auto& arg : m_args) {
        os << " " << arg;
    }
    os << ") {" << std::endl;
    for (const auto &bb : m_basic_blocks) {
        bb.print(os);
    }
    os << std::endl;
    os << "}" << std::endl;

}
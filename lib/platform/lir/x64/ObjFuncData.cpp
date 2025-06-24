#include "ObjFuncData.h"

#include <ostream>


MachBlock * ObjFuncData::last() const {
    const auto last_bb = m_basic_blocks.back();
    return last_bb.value();
}


void ObjFuncData::print(std::ostream &os) const {
    os << m_name << ":" << std::endl;
    for (const auto &bb : m_basic_blocks) {
        bb.print(os);
    }
    os << std::endl;
    os << "}" << std::endl;

}
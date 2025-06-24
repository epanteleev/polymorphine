#include "platform/lir/x64/Chain.h"

void Chain::add_use(const LIROperand& use) {
    m_uses.push_back(use);
}

void Chain::add_def(const VReg& def) {
    m_defs.push_back(def);
}

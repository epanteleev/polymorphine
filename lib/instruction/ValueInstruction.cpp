#include "ValueInstruction.h"

void ValueInstruction::add_user(const Instruction *user) {
    m_users.push_back(user);
}
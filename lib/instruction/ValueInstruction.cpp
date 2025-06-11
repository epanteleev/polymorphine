#include "ValueInstruction.h"

void ValueInstruction::add_user(Instruction *user) {
    m_users.push_back(user);
}
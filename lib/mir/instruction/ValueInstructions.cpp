#include "ValueInstruction.h"
#include "mir/value/Value.h"

void ValueInstruction::replace_all_uses(const Value &new_val) noexcept {
    for (auto* user : release_users()) {
        const auto idx = index_of(user->operands(), this);
        assertion(idx != user->operands().size(), "out of range");
        user->raw_update_operand(idx, new_val);
    }
}
#include "JoinPointSet.h"

void JoinPointSet::run() {

}

bool JoinPointSet::has_user_in_block(const BasicBlock *block, const Alloc *alloc) noexcept {
    if (alloc->owner() == block) {
        return true;
    }

    for (auto& user: alloc->users()) {
        if (!user->isa(store())) {
            continue;
        }

        if (user->owner() == block) {
            return true;
        }
    }

    return false;
}


#include "SccpLattice.h"

#include "mir/value/Value.h"
#include "mir/value/VConstant.h"

namespace details {
    LatticeValue SccpLattice::lattice_of_operand(const Value &operand) const noexcept {
        const auto cst = VConstant::try_from(operand);
        if (cst.has_value()) {
            return LatticeValue::constant(cst.value());
        }

        if (!operand.is<ValueInstruction*>()) {
            return LatticeValue::overdefined();
        }

        const auto* def = operand.get<ValueInstruction*>();
        if (const auto it = m_states.find(def); it != m_states.end()) {
            return it->second;
        }

        return LatticeValue::unknown();
    }

    bool SccpLattice::merge_state(const ValueInstruction *inst, const LatticeValue &incoming) {
        auto& current = m_states[inst];
        if (current.kind() == LatticeKind::Overdefined || incoming.kind() == LatticeKind::Unknown) {
            return false;
        }

        if (incoming.kind() == LatticeKind::Overdefined) {
            if (current.kind() == LatticeKind::Overdefined) {
                return false;
            }
            current = incoming;
            return true;
        }

        // incoming is constant.
        if (current.kind() == LatticeKind::Unknown) {
            current = incoming;
            return true;
        }
        if (current.kind() == LatticeKind::Constant) {
            if (current.cst() == incoming.cst()) {
                return false;
            }
            current = LatticeValue::overdefined();
            return true;
        }

        return false;
    }
}


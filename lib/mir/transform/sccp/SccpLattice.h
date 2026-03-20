#pragma once

#include <unordered_map>

#include "LatticeValue.h"
#include "mir/module/BasicBlock.h"

namespace details {
    class SccpLattice final {
    public:
        using map = std::unordered_map<const ValueInstruction*, LatticeValue>;
        using const_iterator = map::const_iterator;

        [[nodiscard]]
        LatticeValue lattice_of_operand(const Value& operand) const noexcept;

        [[nodiscard]]
        bool merge_state(const ValueInstruction* inst, const LatticeValue &incoming);

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return m_states.begin();
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return m_states.end();
        }

    private:
        std::unordered_map<const ValueInstruction*, LatticeValue> m_states;
    };
}

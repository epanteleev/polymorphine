#pragma once
#include <limits>

#include "utility/Error.h"

template<typename BB>
class CommonInstruction {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    [[nodiscard]]
    std::size_t id() const noexcept {
        assertion(m_id != NO_ID, "Instruction ID is not set");
        return m_id;
    }

    [[nodiscard]]
    BB *owner() const noexcept {
        assertion(m_owner != nullptr, "Instruction owner is not set");
        return m_owner;
    }

protected:
    void connect(const std::size_t id, BB* owner) {
        assertion(m_id == NO_ID, "id is set");
        assertion(m_owner == nullptr, "block owner is set");
        m_id = id;
        m_owner = owner;
    }

    BB* m_owner{};
    std::size_t m_id{NO_ID};
};
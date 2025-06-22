#pragma once

#include <cstddef>
#include <deque>
#include <span>
#include <vector>



class MachBlock final {
public:
    explicit MachBlock(std::size_t id) noexcept
        : m_id(id) {}

    [[nodiscard]]
std::size_t id() const { return m_id; }


    [[nodiscard]]
    std::span<MachBlock* const> successors() const {
        //return last().targets();
    }

    [[nodiscard]]
    std::span<MachBlock* const> predecessors() const {
        return m_predecessors;
    }


private:
    const std::size_t m_id;
    std::vector<MachBlock *> m_predecessors;
    std::deque<MachBlock> m_instructions;
};

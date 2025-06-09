#pragma once

#include <vector>
#include <iosfwd>

#include "instruction/Instruction.h"
#include "utility/OrderedSet.h"

class BasicBlock final {
public:
    template<typename U>
    using creator = std::function<std::unique_ptr<U>(std::size_t, BasicBlock*)>;

    explicit BasicBlock(const std::size_t id): m_id(id) {}

    template<typename U>
    U* push_back(const creator<U> fn) {
        auto creator = [&] (std::size_t id) {
            return fn(id, this);
        };

        return m_instructions.push_back<U>(creator);
    }

    [[nodiscard]]
    std::size_t id() const { return m_id; }

    void print(std::ostream &os) const;

private:
    const std::size_t m_id;
    std::vector<BasicBlock *> m_predecessors;
    std::vector<BasicBlock *> m_successors;
    OrderedSet<Instruction> m_instructions;
};

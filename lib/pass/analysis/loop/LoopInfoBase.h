#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "pass/Constrains.h"

template<CodeBlock BB>
class LoopBlock final {
public:
    using iterator = typename std::unordered_set<BB*>::iterator;

    LoopBlock(const BB* exit, const BB* enter, std::unordered_set<BB*>&& body):
        m_exit(exit),
        m_enter(enter),
        m_body(std::move(body)) {}

    auto begin() const {
        return m_body.begin();
    }

    auto end() const {
        return m_body.end();
    }

    [[nodiscard]]
    const BB* exit() const noexcept {
        return m_exit;
    }

    [[nodiscard]]
    const BB* enter() const noexcept {
        return m_enter;
    }

private:
    const BB* m_exit;
    const BB* m_enter;
    const std::unordered_set<BB*> m_body;
};

template<CodeBlock BB>
class LoopInfoBase final: public AnalysisPassResult {
public:
    explicit LoopInfoBase(std::unordered_map<BB*, std::vector<LoopBlock<BB>>>&& loops):
        m_loops(std::move(loops)) {}

    [[nodiscard]]
    std::span<LoopBlock<BB> const> loops(BB* header) const {
        const auto loop = m_loops.find(header);
        if (loop == m_loops.end()) {
            return {};
        }

        return loop->second;
    }

private:
    const std::unordered_map<BB*, std::vector<LoopBlock<BB>>> m_loops;
};
#pragma once

#include <vector>
#include <ostream>
#include <unordered_map>
#include <unordered_set>

#include "pass/Constrains.h"

template<CodeBlock BB>
class LoopBlock final {
public:
    using const_iterator = typename std::unordered_set<BB*>::const_iterator;

    LoopBlock(const BB* exit, const BB* enter, std::unordered_set<const BB*>&& body):
        m_exit(exit),
        m_enter(enter),
        m_body(std::move(body)) {}

    const_iterator begin() const {
        return m_body.begin();
    }

    const_iterator end() const {
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

    template<CodeBlock BB_>
    friend std::ostream& operator<<(std::ostream& os, const LoopBlock<BB_>& info);

private:
    const BB* m_exit;
    const BB* m_enter;
    const std::unordered_set<const BB*> m_body;
};

template<CodeBlock BB>
std::ostream& operator<<(std::ostream& os, const LoopBlock<BB>& info) {
    os << "  exit: ";
    info.m_exit->print_short_name(os) << std::endl;
    os << "  enter: ";
    info.m_enter->print_short_name(os) << std::endl;
    os << "  body: [";
    for (const auto& [idx, bb] : std::ranges::enumerate_view(info.m_body)) {
        if (idx > 0) {
            os << ", ";
        }

        bb->print_short_name(os);
    }
    os << "]" << std::endl;
    return os;
}

template<CodeBlock BB>
class LoopInfoBase final: public AnalysisPassResult {
public:
    explicit LoopInfoBase(std::unordered_map<const BB*, std::vector<LoopBlock<BB>>>&& loops):
        m_loops(std::move(loops)) {}

    [[nodiscard]]
    std::span<LoopBlock<BB> const> loops(BB* header) const {
        const auto loop = m_loops.find(header);
        if (loop == m_loops.end()) {
            return {};
        }

        return loop->second;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_loops.size();
    }

    template<CodeBlock BB_>
    friend std::ostream& operator<<(std::ostream& os, const LoopInfoBase<BB_>& info);

private:
    const std::unordered_map<const BB*, std::vector<LoopBlock<BB>>> m_loops;
};

template<CodeBlock BB>
std::ostream& operator<<(std::ostream& os, const LoopInfoBase<BB>& info) {
    os << '{' << std::endl;
    for (const auto& [header, loops]: info.m_loops) {
        os << " header: ";
        header->print_short_name(os) << std::endl;
        os << " loops: [" << std::endl;
        for (const auto& loop : loops) {
            os << loop;
        }
        os << " ]" << std::endl;
    }
    os << '}' << std::endl;
    return os;
}
#pragma once

#include <unordered_map>

#include "EscapeState.h"
#include "base/analysis/AnalysisPass.h"
#include "mir/mir_frwd.h"

class EscapeAnalysisResult final: public AnalysisPassResult {
public:
    explicit EscapeAnalysisResult(std::unordered_map<const ValueInstruction*, EscapeState>&& escape_state) noexcept:
        m_escape_state(std::move(escape_state)) {}

    [[nodiscard]]
    EscapeState escape_state(const ValueInstruction* value) const noexcept {
        const auto it = m_escape_state.find(value);
        if (it == m_escape_state.end()) {
            return EscapeState::UNKNOWN;
        }

        return it->second;
    }

private:
    std::unordered_map<const ValueInstruction*, EscapeState> m_escape_state;
};
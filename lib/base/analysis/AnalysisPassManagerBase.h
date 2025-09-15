#pragma once

#include <array>
#include <memory>

#include "AnalysisPass.h"
#include "base/FunctionDataBase.h"

template<Function FD>
class AnalysisPassManagerBase final {
    constexpr static auto MAX_ANALYSIS_PASSES = static_cast<std::size_t>(AnalysisType::Max);

public:
    template <Analysis A>
    A::result_type* analyze(const FD* data) {
        using result_type = A::result_type;
        constexpr auto idx = static_cast<std::size_t>(A::analysis_kind);

        auto& pass_res = m_passes[idx];
        if (pass_res != nullptr) {
            return static_cast<result_type*>(pass_res.get());
        }

        auto a = A::create(this, data);
        a.run();
        pass_res = a.result();
        return static_cast<result_type*>(pass_res.get());
    }

private:
    std::array<std::unique_ptr<AnalysisPassResult>, MAX_ANALYSIS_PASSES> m_passes{};
};
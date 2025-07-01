#pragma once

#include <array>
#include <future>
#include <memory>

#include "AnalysisPass.h"
#include "pass/Constrains.h"


template<Function FD>
class AnalysisPassCacheBase final {
    constexpr static auto MAX_ANALYSIS_PASSES = static_cast<std::size_t>(AnalysisType::Max);

public:
    template <Analysis A>
    typename A::result_type* analyze(const FD* data) {
        using result_type = typename A::result_type;
        constexpr auto idx = static_cast<std::size_t>(A::analysis_kind);

        auto& mutex = m_mutexes[idx];
        std::lock_guard _l(mutex);

        auto& pass_res = m_passes[idx];
        if (pass_res != nullptr) {
            return static_cast<result_type*>(pass_res.get());
        }

        auto a = A::create(this, data);
        a.run();
        pass_res = a.result();
        return static_cast<result_type*>(pass_res.get());
    }

    template <Analysis A>
    std::future<typename A::result_type*> concurrent_analyze(const FD* data) {
        return std::async(std::launch::async, &AnalysisPassCacheBase::analyze<A>, this, data);
    }

private:
    std::array<std::mutex, MAX_ANALYSIS_PASSES> m_mutexes{};
    std::array<std::shared_ptr<AnalysisPassResult>, MAX_ANALYSIS_PASSES> m_passes{};
};

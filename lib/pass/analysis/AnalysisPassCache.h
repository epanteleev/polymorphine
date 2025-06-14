#pragma once
#include <array>
#include <future>
#include <memory>
#include "AnalysisPass.h"

class AnalysisPassCache final {
    constexpr static auto MAX_ANALYSIS_PASSES = static_cast<std::size_t>(AnalysisType::Max);

public:
    template <typename A>
    requires IsAnalysis<A>
    typename A::result_type* analyze(const FunctionData* data) {
        constexpr auto idx = static_cast<std::size_t>(A::analysis_kind);
        auto& pass_res = m_passes[idx];
        auto cached = pass_res.load(std::memory_order_acquire);
        if (cached != nullptr && cached != UNDER_EVAL) {
            return static_cast<typename A::result_type*>(cached.get());
        }

        while (!pass_res.compare_exchange_strong(cached, UNDER_EVAL, std::memory_order_acq_rel)) {
            if (cached != nullptr) {
                return static_cast<typename A::result_type*>(cached.get());
            }

            std::this_thread::yield();
        }

        auto a = A::create(this, data);
        a.run();
        auto result = a.result();
        auto ret = result.get();
        pass_res.store(std::move(result), std::memory_order_release);
        return ret;
    }

    template <typename A>
    requires IsAnalysis<A>
    std::future<typename A::result_type*> concurrent_analyze(const FunctionData* data) {
        return std::async(std::launch::async, &AnalysisPassCache::analyze<A>, this, data);
    }

private:
    static std::shared_ptr<AnalysisPassResult> UNDER_EVAL;
    std::array<std::atomic<std::shared_ptr<AnalysisPassResult>>, MAX_ANALYSIS_PASSES> m_passes{};
};

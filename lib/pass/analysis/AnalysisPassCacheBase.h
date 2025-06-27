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
    template <typename A>
    requires IsAnalysis<A>
    typename A::result_type* analyze(const FD* data) {
        constexpr auto idx = static_cast<std::size_t>(A::analysis_kind);

        auto& pass_res = m_passes[idx];
        auto& flag = m_flags[idx];
        if (flag.load(std::memory_order_relaxed) == 2) {
            const auto res = pass_res.load(std::memory_order_relaxed);
            return static_cast<typename A::result_type*>(res.get());
        }

        auto loaded_flag = flag.load(std::memory_order_relaxed);
        while (!flag.compare_exchange_strong(loaded_flag, 1)) {
            if (loaded_flag == 2) {
                const auto res = pass_res.load(std::memory_order_relaxed);
                return static_cast<typename A::result_type*>(res.get());
            }

            std::this_thread::yield();
        }

        auto a = A::create(this, data);
        a.run();
        auto result = a.result();
        const auto ret = result.get();
        pass_res.store(result, std::memory_order_release);
        flag.store(2, std::memory_order_release);
        return ret;
    }

    template <typename A>
    requires IsAnalysis<A>
    std::future<typename A::result_type*> concurrent_analyze(const FD* data) {
        return std::async(std::launch::async, &AnalysisPassCacheBase::analyze<A>, this, data);
    }

private:
    std::array<std::atomic<std::uint8_t>, MAX_ANALYSIS_PASSES> m_flags{};
    std::array<std::atomic<std::shared_ptr<AnalysisPassResult>>, MAX_ANALYSIS_PASSES> m_passes{};
};

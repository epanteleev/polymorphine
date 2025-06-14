#pragma once

#include <memory>
#include <vector>

#include "Ordering.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCache.h"


class PreorderTraverse final : public AnalysisPass {
    explicit PreorderTraverse(const FunctionData *data) noexcept
        : AnalysisPass(data) {}

public:
    using result_type = Ordering;
    static constexpr auto analysis_kind = AnalysisType::PreorderTraverse;

    void run() override;

    std::shared_ptr<Ordering> result() noexcept {
        return std::make_shared<Ordering>(std::move(m_order));
    }

    static PreorderTraverse create(AnalysisPassCache* cache, const FunctionData* data) {
        return PreorderTraverse(data);
    }

private:
    std::vector<BasicBlock*> m_order{};
};

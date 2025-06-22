#pragma once

#include <algorithm>
#include <memory>
#include <utility>

#include "Ordering.h"
#include "PreorderTraverse.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCache.h"


class PostOrderTraverse final : public AnalysisPass {
    explicit PostOrderTraverse(const FunctionData *data, Ordering &preorder) noexcept
        : AnalysisPass(data),
          m_order(preorder.size()),
          m_preorder(preorder) {}

public:
    using result_type = Ordering;
    static constexpr auto analysis_kind = AnalysisType::PostorderTraverse;

    void run() override {
        std::ranges::reverse_copy(m_preorder, std::begin(m_order));
    }

    std::shared_ptr<Ordering> result() noexcept {
        return std::make_shared<Ordering>(std::move(m_order));
    }

    static PostOrderTraverse create(AnalysisPassCache* cache, const FunctionData *data) {
        auto preorder = cache->concurrent_analyze<PreorderTraverse>(data);
        preorder.wait();
        return PostOrderTraverse(data, *preorder.get());
    }

private:
    std::vector<BasicBlock *> m_order{};
    Ordering &m_preorder;
};

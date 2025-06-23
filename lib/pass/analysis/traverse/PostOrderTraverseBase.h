#pragma once

#include <algorithm>
#include <memory>
#include <utility>

#include "Ordering.h"
#include "PreorderTraverseBase.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCache.h"

template<CodeBlock BB>
class PostOrderTraverseBase final : public AnalysisPass {
public:
    using result_type = Ordering<BB>;

private:
    explicit PostOrderTraverseBase(const FunctionData *data, result_type &preorder) noexcept
        : AnalysisPass(data),
          m_order(preorder.size()),
          m_preorder(preorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::PostOrderTraverse;

    void run() override {
        std::ranges::reverse_copy(m_preorder, std::begin(m_order));
    }

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_order));
    }

    static PostOrderTraverseBase create(AnalysisPassCache* cache, const FunctionData *data) {
        auto preorder = cache->concurrent_analyze<PreorderTraverseBase<BB>>(data);
        preorder.wait();
        return PostOrderTraverseBase(data, *preorder.get());
    }

private:
    std::vector<BB *> m_order{};
    result_type &m_preorder;
};

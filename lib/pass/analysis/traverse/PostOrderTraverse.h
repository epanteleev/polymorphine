#pragma once
#include <memory>
#include <utility>

#include "Ordering.h"
#include "PreorderTraverse.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCache.h"


class PostOrderTraverse final: public AnalysisPass {
public:
    using result_type = Ordering;
    static constexpr auto analysis_kind = AnalysisType::PostorderTraverse;

    explicit PostOrderTraverse(const FunctionData *data, Ordering &preorder) noexcept
        : AnalysisPass(data),
          m_order(preorder.size()),
          m_preorder(preorder) {}

    void run() override;

    std::shared_ptr<Ordering> result() noexcept {
        return std::make_shared<Ordering>(std::move(m_order));
    }

    static PostOrderTraverse create(AnalysisPassCache* cache, const FunctionData* data);
private:
    std::vector<BasicBlock*> m_order{};
    Ordering& m_preorder;
};

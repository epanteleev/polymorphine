#pragma once

#include <algorithm>
#include <memory>
#include <utility>

#include "Ordering.h"
#include "PreorderTraverseBase.h"
#include "pass/analysis/AnalysisPass.h"

template<Function FD>
class PostOrderTraverseBase final {
public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;

private:
    explicit PostOrderTraverseBase(result_type &preorder) noexcept:
          m_order(preorder.size()),
          m_preorder(preorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::PostOrderTraverse;

    void run() {
        std::ranges::reverse_copy(m_preorder, std::begin(m_order));
    }

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_order));
    }

    static PostOrderTraverseBase create(AnalysisPassCacheBase<FD>* cache, const FD *data) {
        auto preorder = cache->template analyze<PreorderTraverseBase<FD>>(data);
        return PostOrderTraverseBase(*preorder);
    }

private:
    std::vector<const basic_block *> m_order{};
    result_type &m_preorder;
};

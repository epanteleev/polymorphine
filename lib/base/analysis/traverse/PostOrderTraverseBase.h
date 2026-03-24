#pragma once

#include <algorithm>
#include <memory>
#include <utility>

#include "Ordering.h"
#include "PreorderTraverseBase.h"
#include "base/analysis/AnalysisPass.h"

template<Function FD>
class PostOrderTraverseBase final {
public:
    using basic_block = FD::code_block_type;
    using result_type = Ordering<basic_block>;

private:
    explicit PostOrderTraverseBase(const FD *data, result_type &preorder) noexcept:
          m_order(data->max_possible_block_id()),
          m_preorder(preorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::PostOrderTraverse;

    void run() {
        std::ranges::reverse_copy(m_preorder, std::begin(m_order));
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_order));
    }

    static PostOrderTraverseBase create(AnalysisPassManagerBase<FD>* cache, const FD *data) {
        auto& preorder = cache->template analyze<PreorderTraverseBase<FD>>(data);
        return PostOrderTraverseBase(data, preorder);
    }

private:
    std::vector<basic_block *> m_order{};
    result_type &m_preorder;
};

#pragma once

#include <memory>
#include <vector>
#include <stack>
#include <ranges>

#include "Ordering.h"
#include "base/BlockMask.h"
#include "base/analysis/AnalysisPass.h"
#include "base/analysis/AnalysisPassManagerBase.h"


template<Function FD>
class PreorderTraverseBase final {
    explicit PreorderTraverseBase(const FD& data) noexcept:
        m_data(data) {}

public:
    using basic_block = FD::code_block_type;
    using result_type = Ordering<basic_block>;
    static constexpr auto analysis_kind = AnalysisType::PreOrderTraverse;

    void run() {
        auto visited = BlockMask<basic_block>::blockMask(m_data);
        std::stack<basic_block*> stack;
        stack.push(m_data.first());
        m_order.reserve(m_data.size());
        
        const auto exit = m_data.last();
        while (!stack.empty()) {
            auto bb = stack.top();
            stack.pop();
            if (visited.contains(bb)) {
                continue;
            }
            if (bb == exit) {
                continue;
            }

            visited.emplace(bb);
            m_order.push_back(bb);

            for (auto s: std::ranges::reverse_view(bb->successors())) {
                stack.push(s);
            }
        }

        m_order.push_back(exit);
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_order));
    }

    static PreorderTraverseBase create(AnalysisPassManagerBase<FD>*, const FD *data) {
        return PreorderTraverseBase(*data);
    }

private:
    const FD& m_data;
    std::vector<basic_block *> m_order{};
};

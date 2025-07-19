#pragma once

#include <memory>
#include <vector>
#include <stack>
#include <ranges>

#include "Ordering.h"
#include "base/analysis/AnalysisPass.h"
#include "base/analysis/AnalysisPassManagerBase.h"


template<Function FD>
class PreorderTraverseBase final {
    explicit PreorderTraverseBase(const FD *data) noexcept:
        m_data(data) {}

public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;
    static constexpr auto analysis_kind = AnalysisType::PreOrderTraverse;

    void run() {
        std::vector visited(m_data->size(), false);
        std::stack<basic_block*> stack;
        stack.push(m_data->first());

        const auto exit = m_data->last();
        while (!stack.empty()) {
            auto bb = stack.top();
            stack.pop();
            if (visited[bb->id()]) {
                continue;
            }
            if (bb == exit) {
                continue;
            }

            visited[bb->id()] = true;
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
        return PreorderTraverseBase(data);
    }

private:
    const FD *m_data;
    std::vector<const basic_block *> m_order{};
};

#pragma once

#include <memory>
#include <vector>
#include <stack>

#include "Ordering.h"
#include "pass/analysis/AnalysisPass.h"

#include "module/FunctionData.h"


template<Function FD>
class PreorderTraverseBase final : public AnalysisPass {
    explicit PreorderTraverseBase(const FunctionData *data) noexcept
        : AnalysisPass(data) {}

public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;
    static constexpr auto analysis_kind = AnalysisType::PreOrderTraverse;

    void run() override {
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

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_order));
    }

    static PreorderTraverseBase create(AnalysisPassCacheBase<FD> *cache, const FunctionData *data) {
        return PreorderTraverseBase(data);
    }

private:
    std::vector<basic_block *> m_order{};
};

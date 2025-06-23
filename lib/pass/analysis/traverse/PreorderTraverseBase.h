#pragma once

#include <memory>
#include <vector>
#include <stack>

#include "Ordering.h"
#include "pass/analysis/AnalysisPass.h"

#include "module/FunctionData.h"


template<CodeBlock BB>
class PreorderTraverseBase final : public AnalysisPass {
    explicit PreorderTraverseBase(const FunctionData *data) noexcept
        : AnalysisPass(data) {}

public:
    using result_type = Ordering<BB>;
    static constexpr auto analysis_kind = AnalysisType::PreOrderTraverse;

    void run() override {
        std::vector visited(m_data->size(), false);
        std::stack<BB*> stack;
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

            stack.push_range(std::ranges::reverse_view(bb->successors()));
        }

        m_order.push_back(exit);
    }

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_order));
    }

    static PreorderTraverseBase create(AnalysisPassCache *cache, const FunctionData *data) {
        return PreorderTraverseBase(data);
    }

private:
    std::vector<BB *> m_order{};
};

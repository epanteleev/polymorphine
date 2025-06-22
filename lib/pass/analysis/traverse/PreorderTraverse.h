#pragma once

#include <memory>
#include <vector>
#include <stack>

#include "Ordering.h"
#include "pass/analysis/AnalysisPass.h"

#include "module/BasicBlock.h"
#include "module/FunctionData.h"


class PreorderTraverse final : public AnalysisPass {
    explicit PreorderTraverse(const FunctionData *data) noexcept
        : AnalysisPass(data) {}

public:
    using result_type = Ordering;
    static constexpr auto analysis_kind = AnalysisType::PreorderTraverse;

    void run() override {
        std::vector visited(m_data->size(), false);
        std::stack<BasicBlock*> stack;
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

    std::shared_ptr<Ordering> result() noexcept {
        return std::make_shared<Ordering>(std::move(m_order));
    }

    static PreorderTraverse create(AnalysisPassCache *cache, const FunctionData *data) {
        return PreorderTraverse(data);
    }

private:
    std::vector<BasicBlock *> m_order{};
};

#pragma once

#include <memory>
#include <stack>

#include "Ordering.h"
#include "module/FunctionData.h"
#include "pass/analysis/AnalysisPass.h"


class BFSOrderTraverse final : public AnalysisPass {
    explicit BFSOrderTraverse(const FunctionData *data) noexcept
        : AnalysisPass(data),
          visited(m_data->size(), false),
          m_order(data->size()) {}

public:
    using result_type = Ordering;
    static constexpr auto analysis_kind = AnalysisType::BFSTraverse;

    void run() override {
        visitBlock(m_data->first());
        while (!stack.empty()) {
            auto bbs = stack.top();
            stack.pop();
            for (const auto bb: bbs) {
                if (visited[bb->id()]) {
                    continue;
                }

                visitBlock(bb);
            }
        }
    }

    std::shared_ptr<Ordering> result() noexcept {
        return std::make_shared<Ordering>(std::move(m_order));
    }

    static BFSOrderTraverse create(AnalysisPassCache *cache, const FunctionData *data) {
        return BFSOrderTraverse(data);
    }

private:
    void visitBlock(BasicBlock *bb) {
        visited[bb->id()] = true;
        m_order.push_back(bb);
        if (!bb->successors().empty()) {
            stack.emplace(bb->successors());
        }
    }

    std::vector<bool> visited;
    std::vector<BasicBlock *> m_order{};
    std::stack<std::span<BasicBlock* const>> stack;
};


#pragma once

#include <memory>
#include <stack>

#include "Ordering.h"
#include "module/FunctionData.h"
#include "pass/analysis/AnalysisPass.h"


template<CodeBlock BB>
class BFSOrderTraverseBase final : public AnalysisPass {
public:
    using result_type = Ordering<BB>;

private:
    explicit BFSOrderTraverseBase(const FunctionData *data) noexcept
        : AnalysisPass(data),
          visited(m_data->size(), false) {}

public:
    static constexpr auto analysis_kind = AnalysisType::BFSTraverse;

    void run() override {
        m_order.reserve(m_data->size());
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

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_order));
    }

    static BFSOrderTraverseBase create(AnalysisPassCache *cache, const FunctionData *data) {
        return BFSOrderTraverseBase(data);
    }

private:
    void visitBlock(BB *bb) {
        visited[bb->id()] = true;
        m_order.push_back(bb);
        if (!bb->successors().empty()) {
            stack.emplace(bb->successors());
        }
    }

    std::vector<bool> visited;
    std::vector<BB *> m_order{};
    std::stack<std::span<BB* const>> stack;
};


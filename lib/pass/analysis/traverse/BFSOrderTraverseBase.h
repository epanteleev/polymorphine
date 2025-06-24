#pragma once

#include <memory>
#include <stack>

#include "Ordering.h"
#include "module/FunctionData.h"
#include "pass/analysis/AnalysisPass.h"


template<Function FD>
class BFSOrderTraverseBase final : public AnalysisPass {
public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;

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

    static BFSOrderTraverseBase create(AnalysisPassCacheBase<FD> *cache, const FunctionData *data) {
        return BFSOrderTraverseBase(data);
    }

private:
    void visitBlock(basic_block *bb) {
        visited[bb->id()] = true;
        m_order.push_back(bb);
        if (!bb->successors().empty()) {
            stack.emplace(bb->successors());
        }
    }

    std::vector<bool> visited;
    std::vector<basic_block *> m_order{};
    std::stack<std::span<basic_block* const>> stack;
};


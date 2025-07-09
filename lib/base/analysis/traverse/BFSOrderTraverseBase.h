#pragma once

#include <memory>
#include <stack>

#include "Ordering.h"
#include "base/analysis/AnalysisPass.h"


template<Function FD>
class BFSOrderTraverseBase final {
public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;

private:
    explicit BFSOrderTraverseBase(const FD *data) noexcept:
        m_data(data),
        visited(data->size(), false) {}

public:
    static constexpr auto analysis_kind = AnalysisType::BFSTraverse;

    void run() {
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

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_order));
    }

    static BFSOrderTraverseBase create(AnalysisPassCacheBase<FD>*, const FunctionData *data) {
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

    const FD* m_data;
    std::vector<bool> visited;
    std::vector<const basic_block *> m_order{};
    std::stack<std::span<basic_block* const>> stack;
};

#pragma once

#include <stack>

#include "Ordering.h"
#include "base/BlockMask.h"
#include "base/analysis/AnalysisPass.h"


template<Function FD>
class BFSOrderTraverseBase final {
public:
    using basic_block = FD::code_block_type;
    using result_type = Ordering<basic_block>;

private:
    explicit BFSOrderTraverseBase(const FD& data) noexcept:
        m_data(data),
        visited(BlockMask<basic_block>::blockMask(data)) {}

public:
    static constexpr auto analysis_kind = AnalysisType::BFSTraverse;

    void run() {
        m_order.reserve(m_data.size());
        visitBlock(m_data.first());
        while (!stack.empty()) {
            auto bbs = stack.top();
            stack.pop();
            for (const auto bb: bbs) {
                if (visited.contains(bb)) {
                    continue;
                }

                visitBlock(bb);
            }
        }
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_order));
    }

    static BFSOrderTraverseBase create(AnalysisPassManagerBase<FD>*, const FD *data) {
        return BFSOrderTraverseBase(*data);
    }

private:
    void visitBlock(basic_block *bb) {
        visited.emplace(bb);
        m_order.push_back(bb);
        if (!bb->successors().empty()) {
            stack.emplace(bb->successors());
        }
    }

    const FD& m_data;
    BlockMask<basic_block> visited;
    std::vector<basic_block *> m_order{};
    std::stack<std::span<basic_block* const>> stack;
};

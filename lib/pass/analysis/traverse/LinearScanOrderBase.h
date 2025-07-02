#pragma once

#include <memory>
#include <ranges>
#include <stack>

#include "Ordering.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCacheBase.h"
#include "pass/analysis/loop/LoopInfoBase.h"

template<Function FD>
class LinearScanOrderBase final {
public:
    using basic_block = typename FD::code_block_type;
    using result_type = Ordering<basic_block>;

private:
    explicit LinearScanOrderBase(const FD *data, const LoopInfoBase<basic_block>* loop_info) noexcept
        : m_data(data),
        m_loop_info(loop_info) {}

public:
    static constexpr auto analysis_kind = AnalysisType::LinearScanOrderTraverse;

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

            const auto loops = m_loop_info->loops(bb);
            if (loops.empty()) {
                continue;
            }

            const auto first = loops.front();
        //    stack.push(first.exit());
         //   stack.push(first.enter());
        }

        m_order.push_back(exit);
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_order));
    }

    static LinearScanOrderBase create(AnalysisPassCacheBase<FD> *cache, const FD *data) {
        const auto loop_info = cache->template analyze<LoopInfoEvalBase<FD>>(data);
        return LinearScanOrderBase(data, loop_info);
    }

private:
    const FD* m_data;
    const LoopInfoBase<basic_block>* m_loop_info;
    std::vector<const basic_block *> m_order{};
};

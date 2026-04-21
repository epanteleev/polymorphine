#pragma once

#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

#include "DominatorTree.h"
#include "base/analysis/AnalysisPass.h"
#include "base/analysis/traverse/Ordering.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"


/**
 * Cooper-Harvey-Kennedy "A Simple, Fast Dominance Algorithm".
 *
 * Blocks are addressed by their post-order index; the entry block has the
 * largest index (size - 1). The idom mapping is refined in reverse post-order
 * (skipping the entry) until it stabilises.
 *
 * Because indices are dense (0..N-1), dominators and predecessors are stored in
 * std::vector rather than std::unordered_map, which removes hashing overhead
 * from the innermost intersect/compute_idom loops.
 */
template<Function FD>
class DominatorTreeEvalBase final {
public:
    using basic_block = FD::code_block_type;
    using order_type = Ordering<basic_block>;
    using result_type = DominatorTree<basic_block>;
    using dom_node = result_type::dom_node;

private:
    explicit DominatorTreeEvalBase(order_type& postorder) noexcept:
        m_postorder(postorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::DominatorTree;

    void run() {
        const auto n = m_postorder.size();
        if (n == 0) {
            return;
        }

        std::vector<basic_block*> index_to_block;
        index_to_block.reserve(n);
        std::unordered_map<basic_block*, std::size_t> block_to_index;
        block_to_index.reserve(n);
        for (const auto bb: m_postorder) {
            block_to_index.emplace(bb, index_to_block.size());
            index_to_block.push_back(bb);
        }

        const auto predecessors = build_predecessors(index_to_block, block_to_index);

        const auto entry = n - 1;
        std::vector<std::size_t> idoms(n, UNDEFINED);
        idoms[entry] = entry;

        bool changed = true;
        while (changed) {
            changed = false;
            for (std::size_t i = entry; i-- > 0;) {
                const auto new_idom = compute_idom(idoms, predecessors[i]);
                if (new_idom != idoms[i]) {
                    idoms[i] = new_idom;
                    changed = true;
                }
            }
        }

        build_dominator_tree(idoms, index_to_block);
    }

    static DominatorTreeEvalBase create(AnalysisPassManagerBase<FD> *cache, const FD *data) {
        auto& post_order = cache->template analyze<PostOrderTraverseBase<FD>>(data);
        return DominatorTreeEvalBase(post_order);
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<result_type>(std::move(m_dominator_tree));
    }

private:
    static constexpr auto UNDEFINED = std::numeric_limits<std::size_t>::max();

    static std::vector<std::vector<std::size_t>> build_predecessors(const std::vector<basic_block*>& index_to_block, const std::unordered_map<basic_block*, std::size_t>& block_to_index) {
        std::vector<std::vector<std::size_t>> predecessors(index_to_block.size());
        for (std::size_t i{}; i < index_to_block.size(); ++i) {
            const auto pred = index_to_block[i]->predecessors();
            auto& bucket = predecessors[i];
            bucket.reserve(pred.size());
            for (const auto p: pred) {
                bucket.push_back(block_to_index.at(p));
            }
        }

        return predecessors;
    }

    /** Walk up the dominator tree from @p a and @p b until they meet. */
    static std::size_t intersect(const std::span<std::size_t const> idoms, std::size_t a, std::size_t b) noexcept {
        while (a != b) {
            while (a < b) {
                a = idoms[a];
            }
            while (b < a) {
                b = idoms[b];
            }
        }

        return a;
    }

    /** Meet over the already-processed predecessors of a block. */
    static std::size_t compute_idom(const std::span<std::size_t const> idoms, const std::span<std::size_t const> preds) noexcept {
        auto it = preds.begin();
        const auto end = preds.end();

        while (it != end && idoms[*it] == UNDEFINED) {
            ++it;
        }
        assertion(it != end, "block must have at least one processed predecessor");

        auto acc = *it++;
        for (; it != end; ++it) {
            if (idoms[*it] != UNDEFINED) {
                acc = intersect(idoms, acc, *it);
            }
        }

        return acc;
    }

    void build_dominator_tree(const std::vector<std::size_t>& idoms, const std::vector<basic_block*>& index_to_block) {
        const auto n = index_to_block.size();
        m_dominator_tree.reserve(n);
        for (const auto bb: index_to_block) {
            m_dominator_tree.emplace(bb, bb);
        }

        const auto entry = n - 1;
        for (std::size_t i{}; i < entry; ++i) {
            const auto block = index_to_block[i];
            const auto idom_block = index_to_block[idoms[i]];
            auto& node = m_dominator_tree.find(block)->second;
            auto& idom_node = m_dominator_tree.find(idom_block)->second;
            node.idom = &idom_node;
            idom_node.children.push_back(block);
        }
    }

    std::unordered_map<basic_block*, dom_node> m_dominator_tree{};
    order_type& m_postorder;
};
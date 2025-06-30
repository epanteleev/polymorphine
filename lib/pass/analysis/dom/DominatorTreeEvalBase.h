#pragma once

#include <memory>
#include <unordered_map>
#include <ranges>
#include <vector>

#include "DominatorTree.h"
#include "pass/analysis/AnalysisPass.h"
#include "ir_frwd.h"
#include "pass/analysis/traverse/Ordering.h"
#include "pass/analysis/traverse/PostOrderTraverseBase.h"


template<Function FD>
class DominatorTreeEvalBase final: public AnalysisPass {
public:
    using basic_block = typename FD::code_block_type;
    using order_type = Ordering<basic_block>;
    using dom_node = DominatorNode<basic_block>;
    using result_type = DominatorTree<basic_block>;

private:
    DominatorTreeEvalBase(const FunctionData* data, order_type& postorder) noexcept
        : AnalysisPass(data),
        m_postorder(postorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::DominatorTree;

    void run() override {
        const auto blocks_indexes = indexing_blocks(m_postorder);
        const auto predecessor_map = calculate_incoming(m_postorder, blocks_indexes);
        auto dominators = initialize_dominators(m_postorder);

        bool changed = true;
        while (changed) {
            changed = false;
            for (auto i: std::views::iota(0UL, m_postorder.size()-1) | std::views::reverse) {
                const auto idom = evaluate_idom(dominators, predecessor_map, i);
                if (idom != dominators.at(i)) {
                    dominators[i] = idom;
                    changed = true;
                }
            }
        }

        auto index_to_label = eval_index_to_label(blocks_indexes);
        enumeration_to_dom_map(m_postorder, index_to_label, dominators);
    }

    static DominatorTreeEvalBase create(AnalysisPassCacheBase<FD> *cache, const FunctionData *data) {
        auto post_order = cache->template analyze<PostOrderTraverseBase<FD>>(data);
        return {data, *post_order};
    }

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(dominator_tree));
    }

private:
    static constexpr auto UNDEFINED = std::numeric_limits<std::size_t>::max();

    void enumeration_to_dom_map(const Ordering<basic_block>& ordering, const std::unordered_map<std::size_t, basic_block*>& index_to_block, std::unordered_map<std::size_t, std::size_t>& dominators) {
        for (const auto key: dominators | std::views::keys) {
            const auto block = index_to_block.at(key);
            dominator_tree[block] = std::make_unique<dom_node>(block);
        }

        dominators.erase(ordering.size()-1); // Remove the entry block

        for (auto& [key, value]: dominators) {
            const auto block = index_to_block.at(key);
            const auto idom = index_to_block.at(value);
            auto& idom_node = dominator_tree[idom];
            auto& dominator_node = dominator_tree[block];

            dominator_node->idom = idom_node.get();
            idom_node->children.push_back(dominator_node.get());
        }
    }

    static std::unordered_map<std::size_t, std::size_t> initialize_dominators(const order_type& postorder) {
        std::unordered_map<std::size_t, std::size_t> dominators;
        const auto begin = postorder.size()-1; // Exclude the entry block
        for (const auto bb : std::views::iota(0UL, postorder.size())) {
            if (bb == begin) {
                dominators[bb] = begin;
            } else {
                dominators[bb] = UNDEFINED;
            }
        }

        return dominators;
    }

    static std::unordered_map<std::size_t, std::vector<std::size_t>> calculate_incoming(const order_type& postorder, const std::unordered_map<basic_block*, std::size_t>& incoming) {
        std::unordered_map<std::size_t, std::vector<std::size_t>> predecessors;

        for (const auto bb : postorder) {
            const auto pred = bb->predecessors();
            if (pred.empty()) {
                continue;
            }

            std::vector<std::size_t> mapped_pred;
            for (const auto p: pred) {
                const auto mapped = incoming.at(p);
                mapped_pred.push_back(mapped);
            }

            const auto key = incoming.at(bb);
            predecessors[key] = std::move(mapped_pred);
        }

        return predecessors;
    }

    static std::unordered_map<basic_block*, std::size_t> indexing_blocks(Ordering<basic_block>& ordering) {
        std::unordered_map<basic_block*, std::size_t> indexing;
        for (auto [i, bb]: std::ranges::views::enumerate(ordering)) {
            indexing[bb] = i;
        }

        return indexing;
    }

    static std::size_t intersect(const std::unordered_map<std::size_t, std::size_t> & dominators, const std::size_t a, const std::size_t b) {
        auto finger1 = a;
        auto finger2 = b;
        while (finger1 != finger2) {
            while (finger1 < finger2) {
                finger1 = dominators.at(finger1);
            }
            while (finger2 < finger1) {
                finger2 = dominators.at(finger2);
            }
        }

        return finger1;
    }

    static std::size_t evaluate_idom(const std::unordered_map<std::size_t, std::size_t> & dominators, const std::unordered_map<std::size_t, std::vector<std::size_t>> & incoming_map, std::size_t idx) {
        const auto& incoming = incoming_map.at(idx);

        const auto filter = [&dominators](const std::size_t idom) {
            return dominators.at(idom) != UNDEFINED;
        };

        const auto fold = [&dominators](const std::size_t acc, const std::size_t idom) {
            return intersect(dominators, acc, idom);
        };

        auto&& v = incoming | std::ranges::views::filter(filter);
        return std::ranges::fold_left_first(v, fold).value();
    }

    static std::unordered_map<std::size_t, basic_block*> eval_index_to_label(const std::unordered_map<basic_block *, std::size_t> & block_to_index) {
        std::unordered_map<std::size_t, basic_block*> index_to_label;
        for (const auto &[bb, idx]: block_to_index) {
            index_to_label[idx] = bb;
        }

        return index_to_label;
    }

    std::unordered_map<basic_block*, std::unique_ptr<dom_node>> dominator_tree{};
    order_type& m_postorder;
};

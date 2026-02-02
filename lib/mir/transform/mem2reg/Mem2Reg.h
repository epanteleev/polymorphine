#pragma once

#include "base/analysis/dom/DominatorTree.h"
#include "mir/analysis/join/JoinPointSet.h"
#include "mir/transform/TransformPass.h"


class Mem2Reg final: public TransformPass {
    explicit Mem2Reg(FunctionData &fn, const DominatorTree<BasicBlock>& tree, const JoinPointSetResult& join_point_set) noexcept:
        m_fn(fn),
        m_tree(tree),
        m_join_point_set(join_point_set) {}

public:
    void run() noexcept override;

    [[nodiscard]]
    std::string_view name() const noexcept override {
        return "mem2reg";
    }

    static Mem2Reg create(FunctionData &fn) noexcept;

private:
    FunctionData& m_fn;
    const DominatorTree<BasicBlock>& m_tree;
    const JoinPointSetResult& m_join_point_set;
};
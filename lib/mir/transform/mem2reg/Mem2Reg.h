#pragma once

#include "mir/analysis/Analysis.h"
#include "mir/transform/TransformPass.h"


class Mem2Reg final: public TransformPass {
    explicit Mem2Reg(FunctionData &fn) noexcept:
        m_fn(fn) {}

public:
    void run() noexcept override;

    [[nodiscard]]
    std::string_view name() const noexcept override {
        return "mem2reg";
    }

    [[nodiscard]]
    static Mem2Reg create(FunctionData &fn) noexcept;

private:
    FunctionData& m_fn;
    AnalysisPassManager m_manager;
};
#pragma once

#include "mir/analysis/Analysis.h"
#include "mir/transform/TransformPass.h"

class Sccp final: public TransformPass {
    explicit Sccp(FunctionData &fn) noexcept:
        m_fn(fn) {}

public:
    void run() noexcept override;

    [[nodiscard]]
    std::string_view name() const noexcept override {
        return "sccp";
    }

    [[nodiscard]]
    static Sccp create(FunctionData &fn) noexcept;

private:
    FunctionData& m_fn;
    AnalysisPassManager m_manager;
};
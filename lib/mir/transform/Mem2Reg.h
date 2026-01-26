#pragma once

#include "mir/module/FunctionData.h"

class Mem2Reg final {
    explicit Mem2Reg(FunctionData &fn) noexcept: m_fn(fn) {}

public:
    static void run(FunctionData& fn) noexcept;

private:
    void run_impl() noexcept;

    FunctionData& m_fn;
};
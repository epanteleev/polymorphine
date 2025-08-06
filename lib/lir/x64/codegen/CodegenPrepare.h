#pragma once
#include "lir/x64/module/LIRModule.h"

class CodegenPrepare final {
public:
    explicit CodegenPrepare(LIRModule& module) noexcept
        : m_module(module) {}

    void run() const;

private:
    LIRModule& m_module;
};

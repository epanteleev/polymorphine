#pragma once

#include "lir/x64/asm/ObjModule.h"
#include "lir/x64/module/LIRModule.h"

class Codegen {
public:
    explicit Codegen(const LIRModule &module) noexcept
        : m_module(module) {}

    void run();

    ObjModule result() {
        return ObjModule(std::move(m_assemblers));
    }

private:
    std::unordered_map<std::string, AsmEmitter> m_assemblers;
    const LIRModule& m_module;
};

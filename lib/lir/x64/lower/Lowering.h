#pragma once

#include "mir/module/Module.h"
#include "mir/analysis/Analysis.h"

#include "lir/x64/module/LIRModule.h"


class Lowering final {
public:
    explicit Lowering(const Module &module) noexcept
        : m_module(module) {}

    void run();

    LIRModule result() {
        return LIRModule(std::move(m_obj_functions));
    }

    static Lowering create(AnalysisPassManager&, const Module &module) {
        return Lowering(module);
    }

private:
    const Module& m_module;
    std::unordered_map<std::string, std::unique_ptr<LIRFuncData>> m_obj_functions;
};

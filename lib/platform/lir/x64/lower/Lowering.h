#pragma once

#include "module/Module.h"
#include "pass/analysis/Analysis.h"
#include "../module/ObjModule.h"


class Lowering final {
public:
    explicit Lowering(Module &module) noexcept
        : m_module(module) {}

    void run();

    ObjModule result() {
        return ObjModule(std::move(m_obj_functions));
    }

    static Lowering create(AnalysisPassCache&, Module &module) {
        return Lowering(module);
    }

private:
    Module& m_module;
    std::unordered_map<std::string, std::unique_ptr<ObjFuncData>> m_obj_functions;
};

#include "Lowering.h"

#include <ranges>

#include "lir/x64/lower/FunctionLower.h"


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        AnalysisPassManager cache;
        auto lower = FunctionLower::create(&cache, func.get());
        lower.run();

        m_obj_functions.emplace(func->name(), lower.result());
    }
}
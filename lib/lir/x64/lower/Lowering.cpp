#include "Lowering.h"
#include "lir/x64/lower/FunctionLower.h"

#include <ranges>


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        AnalysisPassManager cache;
        auto lower = FunctionLower::create(&cache, &func);
        lower.run();

        m_obj_functions.emplace(func.name(), lower.result());
    }
}
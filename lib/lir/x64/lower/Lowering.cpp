#include "lir/x64/lower/Lowering.h"
#include "lir/x64/lower/FunctionLower.h"
#include "lir/x64/asm/cc/LinuxX64.h"
#include "lir/x64/lower/GlobalsLowering.h"

#include <ranges>


void Lowering::lower_globals_pool() {
    for (auto& global: m_module.gvalue_pool() | std::views::values) {
        if (global.kind() == GValueKind::CONSTANT) {
            continue;
        }

        GlobalsLowering lowering{m_global_data, global};
        lowering.lower();
    }
}

void Lowering::lower_functions() {
    for (const auto &func: m_module.functions() | std::views::values) {
        AnalysisPassManager cache;
        auto lower = FunctionLower::create(&cache, &func, m_global_data, call_conv::CC_LinuxX64());
        lower.run();

        m_obj_functions.emplace(func.name(), lower.result());
    }
}

void Lowering::run() {
    lower_globals_pool();
    lower_functions();
}
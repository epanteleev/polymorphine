#include "Lowering.h"

#include <ranges>

#include "lir/x64/lower/FunctionLower.h"


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        std::vector<LIRArg> args;
        args.reserve(func->args().size());

        for (auto [idx, varg]: std::ranges::views::enumerate(func->args())) {
            args.emplace_back(idx, varg.type()->size_of());
        }

        AnalysisPassManager cache;
        auto lower = FunctionLower::create(&cache, func.get());
        lower.run();

        m_obj_functions.emplace(func->name(), lower.result());
    }
}
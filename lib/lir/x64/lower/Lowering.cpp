#include "Lowering.h"

#include <ranges>

#include "FunctionLower.h"
#include "mir/instruction/TerminateInstruction.h"


void Lowering::run() {
    for (const auto &func: m_module.functions() | std::views::values) {
        std::vector<LIRArg> args;
        args.reserve(func->args().size());

        for (auto [idx, varg]: std::ranges::views::enumerate(func->args())) {
            args.emplace_back(idx, varg.type()->size_of());
        }

        auto obj_func = std::make_unique<LIRFuncData>(func->name(), std::move(args));
        FunctionLower lower(*obj_func.get(), *func);
        lower.run();

        m_obj_functions.emplace(func->name(), std::move(obj_func));
    }
}
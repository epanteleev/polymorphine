#include "OptPipeline.h"
#include "mir/module/Module.h"

#include <ranges>

void OptPipeline::run(Module& module) const noexcept {
    for (auto &fn: module.functions() | std::views::values) {
        for (const auto& pass: m_passes) {
            pass(fn);
        }
    }
}

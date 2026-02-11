#pragma once

#include <concepts>
#include <vector>

#include "TransformPass.h"
#include "mir/module/Module.h"
#include "mir/module/FunctionData.h"

class OptPipeline final {
public:
    using PassFn = void(*)(FunctionData&);

    constexpr OptPipeline() = default;

    template<std::derived_from<TransformPass> Pass>
    constexpr OptPipeline& add_pass() {
        m_passes.push_back(&pass<Pass>);
        return *this;
    }

    void run(Module& module) const noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_passes.size();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_passes.empty();
    }

private:
    template<std::derived_from<TransformPass> Pass>
    static void pass(FunctionData& fn) {
        auto pass = Pass::create(fn);
        pass.run();
    }

    std::vector<PassFn> m_passes;
};
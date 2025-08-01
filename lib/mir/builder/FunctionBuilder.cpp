#include "mir/builder/FunctionBuilder.h"

FunctionBuilder::FunctionBuilder(std::unique_ptr<FunctionData>&& functionData): m_fd(std::move(functionData)) {
    m_bb = m_fd->first();
}

FunctionBuilder FunctionBuilder::make(FunctionPrototype &&prototype, std::vector<ArgumentValue> &&args) noexcept {
    return FunctionBuilder(std::make_unique<FunctionData>(std::move(prototype), std::move(args)));
}

std::unique_ptr<FunctionData> FunctionBuilder::build() noexcept {
    const auto& bbs = m_fd->basic_blocks();

    for (const auto& bb : bbs) {
        if (!bb.last().is<Return>() && !bb.last().is<ReturnValue>()) {
            continue;
        }
        if (bbs.back() == &bb) {
            continue;
        }

        auto current = m_fd->remove(&bb);
        m_fd->add_basic_block(std::move(current));
    }
    return std::move(m_fd);
}
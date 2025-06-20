#include "builder/FunctionBuilder.h"

FunctionBuilder::FunctionBuilder(std::unique_ptr<FunctionData> functionData): m_fd(std::move(functionData)) {
    m_bb = m_fd->first();
}

FunctionBuilder FunctionBuilder::make(FunctionPrototype &&prototype, std::vector<ArgumentValue> &&args) noexcept {
    return FunctionBuilder(std::make_unique<FunctionData>(std::move(prototype), std::move(args)));
}

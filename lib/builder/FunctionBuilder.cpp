#include "FunctionBuilder.hpp"

FunctionBuilder::FunctionBuilder(std::unique_ptr<FunctionData> functionData): m_fd(std::move(functionData)) {
    m_bb = m_fd->begin();
}

FunctionBuilder FunctionBuilder::make(std::size_t id, FunctionPrototype &&prototype, std::vector<ArgumentValue> &&args) noexcept {
    return FunctionBuilder(std::make_unique<FunctionData>(id, std::move(prototype), std::move(args)));
}

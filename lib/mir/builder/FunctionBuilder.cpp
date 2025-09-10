#include "mir/builder/FunctionBuilder.h"

FunctionBuilder::FunctionBuilder(FunctionData* functionData): m_fd(functionData) {
    m_bb = m_fd->first();
}
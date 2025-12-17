#pragma once

#include "lir/x64/global/GlobalData.h"
#include "lir/x64/global/LIRSlot.h"
#include "utility/ArithmeticUtils.h"

class GlobalsLowering final {
public:
    explicit GlobalsLowering(GlobalData& global_data, const GlobalValue& global_value) noexcept:
        m_global_data(global_data),
        m_global_value(global_value) {}

    const LIRNamedSlot* lower();

private:
    LIRSlot create_slot_iter(const NonTrivialType *ty, const Initializer &global);
    LIRSlot create_slot(const NonTrivialType* ty, const Initializer& global);
    LIRSlot lower_aggregate_slot(const AggregateType* aggregate_type, const std::vector<Initializer>& initializers);

    GlobalData& m_global_data;
    const GlobalValue &m_global_value;
};
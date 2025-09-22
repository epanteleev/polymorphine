#pragma once

#include "lir/x64/global/GlobalData.h"
#include "lir/x64/global/LIRSlot.h"

class GlobalsLowering final {
public:
    explicit GlobalsLowering(GlobalData& global_data, const GlobalValue& global_value) noexcept:
        m_global_data(global_data),
        m_global_value(global_value) {}


    const LIRNamedSlot* lower();

private:
    LIRSlot create_slot_iter(const NonTrivialType *ty, const Initializer &global);
    LIRSlot create_slot(const NonTrivialType* ty, const Initializer& global);

    GlobalData& m_global_data;
    const GlobalValue &m_global_value;
};

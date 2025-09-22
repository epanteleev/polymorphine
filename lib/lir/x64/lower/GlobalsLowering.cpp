#include <ranges>

#include "mir/global/GlobalValue.h"
#include "mir/types/ArrayType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/TypeMatcher.h"
#include "GlobalsLowering.h"


const LIRNamedSlot* GlobalsLowering::lower() {
    auto slot = create_slot(m_global_value.content_type(), m_global_value.initializer());
    return m_global_data.add_slot(m_global_value.name(), LIRNamedSlot(std::string(m_global_value.name()), std::move(slot))).value();
}

LIRSlot GlobalsLowering::create_slot(const NonTrivialType *ty, const Initializer &global) {
    return create_slot_iter(ty, global);
}

LIRSlot GlobalsLowering::create_slot_iter(const NonTrivialType* ty, const Initializer& global) {
    const auto vis = [&]<typename U>(const U& glob) -> LIRSlot {
        if constexpr (std::is_same_v<U, double>) {
            unimplemented();

        } else if constexpr (std::is_same_v<U, std::int64_t>) {
            const auto int_type = dynamic_cast<const IntegerType*>(ty);
            assertion(int_type != nullptr, "Expected IntegerType for integer constant");
            const auto slot_type = aasm::to_slot_type(int_type->size_of());
            return LIRSlot(glob, slot_type.value());

        } else if constexpr (std::is_same_v<U, std::string>) {
            const auto array_type = dynamic_cast<const ArrayType*>(ty);
            assertion(array_type != nullptr, "Expected ArrayType for string constant");
            assertion(array_type->element_type()->isa(byte_type()), "Expected byte type for string constant");
            assertion(array_type->length() >= glob.size(), "String constant is too large");
            // TODO handle different sizes
            return LIRSlot(glob, aasm::SlotType::String);

        } else if constexpr (std::is_same_v<U, std::vector<Initializer>>) {
            const auto agg_type = dynamic_cast<const AggregateType*>(ty);
            assertion(agg_type != nullptr, "Expected AggregateType for aggregate constant");

            std::vector<LIRSlot> slots;
            slots.reserve(glob.size());
            for (const auto& [idx, field]: std::views::enumerate(glob)) {
                const auto field_type = agg_type->field_type_of(idx);
                slots.push_back(create_slot_iter(field_type, field));
            }
            return LIRSlot(std::move(slots), aasm::SlotType::Aggregate);

        } else if constexpr (std::is_same_v<U, const GlobalValue*>) {
            auto slot = create_slot(glob->content_type(), glob->initializer());
            const auto named_slot = m_global_data.add_slot(glob->name(), LIRNamedSlot(std::string(glob->name()), std::move(slot))).value();
            return LIRSlot(named_slot, aasm::SlotType::QWord);

        } else {
            static_assert(false);
            std::unreachable();
        }
    };

    return global.visit(vis);
}
#include <ranges>

#include "mir/global/GlobalValue.h"
#include "mir/types/ArrayType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/PointerType.h"
#include "mir/types/FloatingPointType.h"
#include "mir/types/TypeMatcher.h"
#include "GlobalsLowering.h"

#include "mir/types/StructType.h"


const LIRNamedSlot* GlobalsLowering::lower() {
    auto slot = create_slot(m_global_value.content_type(), m_global_value.initializer());
    return m_global_data.add_slot(m_global_value.name(), LIRNamedSlot(std::string(m_global_value.name()), std::move(slot))).value();
}

LIRSlot GlobalsLowering::create_slot(const NonTrivialType *ty, const Initializer &global) {
    return create_slot_iter(ty, global);
}

LIRSlot GlobalsLowering::lower_aggregate_slot(const AggregateType *aggregate_type, const std::vector<Initializer> &initializers) {
    std::vector<LIRSlot> slots;
    slots.reserve(initializers.size()+1);
    for (const auto& [idx, field]: std::views::enumerate(initializers)) {
        const auto field_type = aggregate_type->field_type_of(idx);
        slots.push_back(create_slot_iter(field_type, field));
    }

    if (initializers.size() == aggregate_type->length()) {
        return LIRSlot(std::move(slots));
    }

    if (const auto array_type = dynamic_cast<const ArrayType*>(aggregate_type); array_type != nullptr) {
        const auto remaining = aggregate_type->length() - initializers.size();
        const auto bytes = array_type->element_type()->size_of() * remaining;
        slots.emplace_back(ZeroInit(bytes));

    } else if (const auto struct_type = dynamic_cast<const StructType*>(aggregate_type); struct_type != nullptr) {
        const auto begin = struct_type->offset_of(initializers.size());
        const auto end = struct_type->size_of();
        const auto bytes = end - begin;
        slots.emplace_back(ZeroInit(bytes));

    } else {
        std::unreachable();
    }

    return LIRSlot(std::move(slots));
}

LIRSlot GlobalsLowering::create_slot_iter(const NonTrivialType* ty, const Initializer& global) {
    const auto vis = [&]<typename U>(const U& glob) -> LIRSlot {
        if constexpr (std::is_same_v<U, double>) {
            unimplemented();

        } else if constexpr (std::is_same_v<U, std::int64_t>) {
            const auto int_type = dynamic_cast<const IntegerType*>(ty);
            assertion(int_type != nullptr, "Expected IntegerType for integer constant");
            const auto slot_type = to_slot_type(int_type->size_of());
            return LIRSlot(Constant(slot_type, glob));

        } else if constexpr (std::is_same_v<U, std::string>) {
            const auto array_type = dynamic_cast<const ArrayType*>(ty);
            assertion(array_type != nullptr, "Expected ArrayType for string constant");
            assertion(array_type->element_type()->isa(byte_type()), "Expected byte type for string constant");
            assertion(array_type->length() >= glob.size(), "String constant is too large");
            // TODO handle different sizes
            return LIRSlot(glob);

        } else if constexpr (std::is_same_v<U, std::vector<Initializer>>) {
            const auto agg_type = dynamic_cast<const AggregateType*>(ty);
            assertion(agg_type != nullptr, "Expected AggregateType for aggregate constant");
            return lower_aggregate_slot(agg_type, glob);

        } else if constexpr (std::is_same_v<U, const GlobalValue*>) {
            auto slot = create_slot(glob->content_type(), glob->initializer());
            const auto named_slot = m_global_data.add_slot(glob->name(), LIRNamedSlot(std::string(glob->name()), std::move(slot))).value();
            return LIRSlot(named_slot);

        } else {
            static_assert(false);
            std::unreachable();
        }
    };

    return global.visit(vis);
}

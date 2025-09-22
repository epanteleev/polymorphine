#include "Codegen.h"

#include "lir/x64/codegen/LIRFunctionCodegen.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/transform/callinfo/CallInfoInitialize.h"
#include "lir/x64/transform/regalloc/LinearScanBase.h"
#include "asm/global/Directive.h"

aasm::Slot Codegen::convert_lir_slot(const aasm::SlotType type, const LIRSlot& lir_slot) noexcept {
    const auto vis = [&]<typename T>(const T& data) -> aasm::Slot {
        if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::int64_t>) {
            return aasm::Slot(data, type, lir_slot.size());

        } else if constexpr (std::is_same_v<T, std::vector<LIRSlot>>) {
            std::vector<aasm::Slot> slots;
            slots.reserve(data.size());
            for (const auto& slot: data) {
                slots.push_back(convert_lir_slot(slot.type(), slot));
            }

            return aasm::Slot(std::move(slots), type, lir_slot.size());

        } else if constexpr (std::is_same_v<T, const LIRNamedSlot*>) {
            auto slot = convert_lir_slot(data->type(), data->root());
            auto [symbol, _] = m_symbol_table.add(data->name(), aasm::BindAttribute::INTERNAL);
            auto [directive, has] = m_slots.emplace(symbol, aasm::Directive(symbol, std::move(slot)));
            assertion(has, "Slot already exists in Codegen::convert_lir_slot");
            return aasm::Slot(&directive->second, type, lir_slot.size());

        } else {
            static_assert(false, "Unsupported type in Slot::compute_size");
            std::unreachable();
        }
    };

    return lir_slot.visit(vis);
}

void Codegen::convert_lir_slots(const GlobalData& global_data) {
    for (const auto &slot: global_data | std::views::values) {
        auto [symbol, _] = m_symbol_table.add(slot.name(), aasm::BindAttribute::INTERNAL);
        const auto element = m_slots.find(symbol);
        if (element != m_slots.end()) {
            // Slot already exists, skip
            continue;
        }

        auto asm_slot = convert_lir_slot(slot.type(), slot.root());
        m_slots.emplace_hint(element, symbol, aasm::Directive(symbol, std::move(asm_slot)));
    }
}

void Codegen::run() {
    convert_lir_slots(m_module.global_data());

    for (auto& func: m_module | std::views::values) {
        convert_lir_slots(func.global_data());

        LIRAnalysisPassManager cache;
        auto linear_scan = LinearScanBase::create(&cache, &func, call_conv::CC_LinuxX64());
        linear_scan.run();

        auto call_info = CallInfoInitialize::create(&cache, &func, call_conv::CC_LinuxX64());
        call_info.run();

        auto fn_codegen = LIRFunctionCodegen::create(&cache, &func, m_symbol_table);
        fn_codegen.run();

        const auto [symbol, _] = m_symbol_table.add(func.name(), aasm::BindAttribute::INTERNAL);
        const auto [_unused, has] = m_assemblers.emplace(symbol, fn_codegen.result().to_buffer());
        assertion(has, "Function already exists");
    }
}

aasm::AsmModule Codegen::result() {
    return aasm::AsmModule(std::move(m_symbol_table), std::move(m_assemblers), std::move(m_slots));
}
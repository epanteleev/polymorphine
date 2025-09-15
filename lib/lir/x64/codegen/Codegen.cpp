#include "Codegen.h"

#include "lir/x64/codegen/LIRFunctionCodegen.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/transform/callinfo/CallInfoInitialize.h"
#include "lir/x64/transform/regalloc/LinearScanBase.h"

void Codegen::run() {
    for (auto& func: m_module | std::views::values) {
        for (auto& [name, slot]: func.global_data()) {
            const auto [symbol, _] = m_symbol_table->add(name, aasm::BindAttribute::INTERNAL);
            m_slots.emplace(symbol, slot); //Fixme copying slot
        }

        LIRAnalysisPassManager cache;
        auto linear_scan = LinearScanBase::create(&cache, &func, call_conv::CC_LinuxX64());
        linear_scan.run();

        auto call_info = CallInfoInitialize::create(&cache, &func, call_conv::CC_LinuxX64());
        call_info.run();

        auto fn_codegen = LIRFunctionCodegen::create(&cache, &func, *m_symbol_table);
        fn_codegen.run();

        const auto [symbol, _] = m_symbol_table->add(func.name(), aasm::BindAttribute::INTERNAL);
        m_assemblers.emplace(symbol, fn_codegen.result().to_buffer());
    }
}
#include "Codegen.h"

#include "LIRFunctionCodegen.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/transform/callinfo/CallInfoInitialize.h"

void Codegen::run() {
    for (const auto& func: m_module.functions()) {
        LIRAnalysisPassManager cache;

        auto fn_codegen = LIRFunctionCodegen::create(&cache, func.get(), *m_symbol_table);
        fn_codegen.run();

        const auto [symbol, _] = m_symbol_table->add(func->name(), aasm::Linkage::INTERNAL);
        m_assemblers.emplace(symbol, fn_codegen.result().to_buffer());
    }
}

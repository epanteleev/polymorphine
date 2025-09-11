#include "Codegen.h"

#include "lir/x64/codegen/LIRFunctionCodegen.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/transform/Transform.h"
#include "lir/x64/transform/callinfo/CallInfoInitialize.h"

void Codegen::run() {
    for (const auto& func: m_module.functions()) {
        LIRAnalysisPassManager cache;

        auto fixed_reg_eval = FixedRegistersEvalLinuxX64::create(&cache, func.get());
        fixed_reg_eval.run();

        auto call_info = CallInfoInitializeLinuxX64::create(&cache, func.get());
        call_info.run();

        auto fn_codegen = LIRFunctionCodegen::create(&cache, func.get(), *m_symbol_table);
        fn_codegen.run();

        const auto [symbol, _] = m_symbol_table->add(func->name(), aasm::Linkage::INTERNAL);
        m_assemblers.emplace(symbol, fn_codegen.result().to_buffer());
    }
}
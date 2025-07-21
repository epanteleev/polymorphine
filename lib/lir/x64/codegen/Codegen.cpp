#include "Codegen.h"

#include "MachFunctionCodegen.h"
#include "lir/x64/analysis/Analysis.h"

void Codegen::run() {
    for (const auto& func: m_module.functions()) {
        AnalysisPassManagerMach cache;
        auto fn_codegen = MachFunctionCodegen::create(&cache, func.get());
        fn_codegen.run();
        m_assemblers.emplace(func->name(), fn_codegen.result().to_buffer());
    }
}

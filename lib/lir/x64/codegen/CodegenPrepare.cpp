#include "CodegenPrepare.h"

#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/transform/Transform.h"

void CodegenPrepare::run() const {
    for (auto& func: m_module.functions()) {
        LIRAnalysisPassManager cache;
        auto call_info = CallInfoInitializeLinuxX64::create(&cache, func.get());
        call_info.run();
    }
}

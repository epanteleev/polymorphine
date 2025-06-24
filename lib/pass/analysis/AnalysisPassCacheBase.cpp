#include "AnalysisPassCacheBase.h"

std::shared_ptr<AnalysisPassResult> DummyAnalysisPassResult::UNDER_EVAL = std::make_shared<DummyAnalysisPassResult>();
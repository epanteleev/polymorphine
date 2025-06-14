#include "AnalysisPassCache.h"

class DummyAnalysisPassResult final: public AnalysisPassResult {};

std::shared_ptr<AnalysisPassResult> AnalysisPassCache::UNDER_EVAL = std::make_shared<DummyAnalysisPassResult>();
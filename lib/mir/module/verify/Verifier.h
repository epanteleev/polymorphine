#pragma once

#include "mir/module/Module.h"
#include "VerifierResult.h"

class Verifier final {
public:
    [[nodiscard]]
    static std::optional<VerifierResult> apply(const Module& module);
};
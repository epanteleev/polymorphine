#include "Vreg.h"

#include "LIROperand.h"

std::expected<VReg, Error> VReg::from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::BadCastError);
    }

    return vreg.value();
}

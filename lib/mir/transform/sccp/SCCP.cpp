#include "SCCP.h"

void SCCP::run() noexcept {

}

SCCP SCCP::create(FunctionData &fn) noexcept {
    return SCCP(fn);
}

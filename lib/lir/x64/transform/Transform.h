#pragma once

#include "callinfo/CallInfoInitialize.h"
#include "fixedregs/FixedRegistersEvalBase.h"

using CallInfoInitializeLinuxX64 = CallInfoInitialize<call_conv::LinuxX64>;
using FixedRegistersEvalLinuxX64 = FixedRegistersEvalBase<call_conv::LinuxX64>;
#pragma once

#include "builder/FunctionBuilder.h"
#include "builder/ModuleBuilder.h"

#include "module/BasicBlock.h"
#include "module/FunctionData.h"

#include "types/Type.h"
#include "types/StructType.h"
#include "types/VoidType.h"
#include "types/TypeMatcher.h"

#include "instruction/Instruction.h"
#include "instruction/ValueInstruction.h"
#include "instruction/Binary.h"
#include "instruction/Unary.h"
#include "instruction/TerminateInstruction.h"

#include "value/UsedValueMap.h"
#include "value/ValueMatcher.h"

#include "analysis/Analysis.h"

#include "transform/mem2reg/Mem2Reg.h"
#include "transform/sccp/Sccp.h"
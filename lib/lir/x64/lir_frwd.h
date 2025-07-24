#pragma once
#include <cstdint>

class LIRBlock;

class LIRInstructionBase;
class LIRProducerInstruction;
class LIRControlInstruction;
class LIRProducerInstructionBase;
enum class LIRCondType: std::uint8_t;

class LIROperand;
class LIRVal;
class LIRArg;
class LirCst;
class VregBuilder;

class GPVReg;
class GPOp;
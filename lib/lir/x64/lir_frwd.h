#pragma once

#include <cstdint>

class LIRBlock;

class LIRInstructionBase;
class LIRProducerInstruction;
class LIRControlInstruction;
class LIRProducerInstructionBase;
class LIRCall;
enum class FunctionVisibility: std::uint8_t;

class LIROperand;
class LIRVal;
class LIRArg;
class LirCst;
class VregBuilder;

class GPVReg;
class GPOp;

class MasmEmitter;

class TemporalRegs;
class RegisterAllocation;
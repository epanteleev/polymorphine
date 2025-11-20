#pragma once

#include <cstdint>

class LIRBlock;

class LIRInstructionBase;
class LIRProducerInstruction;
class LIRControlInstruction;
class LIRProducerInstructionBase;
class LIRCall;
class LIRAdjustStack;

enum class FunctionBind: std::uint8_t;
enum class FcmpOrdering : std::uint8_t;

class LIROperand;
class LIRVal;
class LIRArg;
class LirCst;
class VregBuilder;

class GPVReg;
class GPOp;

class MasmEmitter;

class TemporalRegs;

class LIRSlot;
class LIRNamedSlot;
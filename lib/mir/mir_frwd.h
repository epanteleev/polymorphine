#pragma once

class Instruction;
class Binary;
class Unary;
class TerminateInstruction;
class Phi;
class ValueInstruction;
class TerminateValueInstruction;
class Return;
class Branch;
class CondBranch;
class Switch;
class ReturnValue;
class VCall;
class IVCall;
class Store;
class Alloc;
class IcmpInstruction;
class GetElementPtr;
class GetFieldPtr;
class Call;
class Select;

class BasicBlock;

class Type;
class IntegerType;
class UnsignedIntegerType;
class FloatingPointType;
class SignedIntegerType;
class PrimitiveType;
class NonTrivialType;
class PointerType;
class FlagType;
class VoidType;
class StructType;
class ArrayType;

class Value;
class ArgumentValue;

class FunctionData;

enum class Error;

class AnalysisPassResult;

class FunctionBuilder;
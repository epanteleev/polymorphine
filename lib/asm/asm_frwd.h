#pragma once

#include <cstdint>

namespace aasm {
    class Address;
    class AsmEmitter;
    class GPReg;
    class Symbol;
    class GPRegSet;
    enum class CondType: std::uint8_t;

    class Slot;
    class Directive;
    class SymbolTable;
}
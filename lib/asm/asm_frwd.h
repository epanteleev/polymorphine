#pragma once

namespace aasm {
    class Address;
    class AsmEmitter;
    class GPReg;
    class Symbol;
    class GPRegSet;
    template<typename V>
    class GPRegMap;
    enum class CondType: std::uint8_t;

    class Slot;
    class Directive;
}
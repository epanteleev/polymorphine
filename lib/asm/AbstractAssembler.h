#pragma once

#include "Address.h"
#include "Register.h"

namespace aasm {
    class AbstractAssembler {
    public:
        virtual ~AbstractAssembler() = default;

        virtual void ret() = 0;

        virtual void popq(GPReg r) = 0;
        virtual void popw(GPReg r) = 0;

        virtual void popq(const Address& addr) = 0;
        virtual void popw(const Address& addr) = 0;

        virtual void pushq(GPReg r) = 0;
        virtual void pushw(GPReg r) = 0;

        virtual void pushq(const Address& addr) = 0;
        virtual void pushw(const Address& addr) = 0;

        virtual void movq(GPReg src, GPReg dest) = 0;
        virtual void movl(GPReg src, GPReg dest) = 0;
        virtual void movw(GPReg src, GPReg dest) = 0;
        virtual void movb(GPReg src, GPReg dest) = 0;
    };
}

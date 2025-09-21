#pragma once

#include "mir/value/Use.h"
#include "mir/global/GlobalSymbol.h"
#include "mir/types/PointerType.h"


class GlobalVariable final: public GlobalSymbol, public Use {
public:
    explicit GlobalVariable(std::string&& name, const NonTrivialType* type, Initializer&& value) noexcept:
        GlobalSymbol(std::move(name), type, std::move(value)),
        Use(PointerType::ptr()) {}

    void print_description(std::ostream& os) const;
};
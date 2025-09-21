#pragma once

#include "mir/global/GlobalSymbol.h"


class GlobalConstant final: public GlobalSymbol {
public:
    explicit GlobalConstant(std::string&& name, const NonTrivialType* type, Initializer&& value) noexcept:
        GlobalSymbol(std::move(name), type, std::move(value)) {}

    void print_description(std::ostream& os) const;
};
#include "Initializer.h"
#include "GlobalSymbol.h"

#include <ostream>
#include <ranges>

std::ostream & operator<<(std::ostream &os, const Initializer &sym) {
    const auto vis = [&]<typename T>(const T& val) {
        if constexpr (std::is_same_v<T, std::vector<Initializer>>) {
            os << '{';
            for (const auto& [idx, v]: std::views::enumerate(val)) {
                if (idx > 0) os << ", ";
                os << v;
            }
            os << '}';

        } else if constexpr (std::is_same_v<T, std::string>) {
            os << '"' << val << '"';

        } else if constexpr (std::is_same_v<T, const GlobalConstant*>) {
            os << *val;

        } else {
            os << val;
        }
    };
    os << '{';
    std::visit(vis, sym.m_value);
    return os << '}';
}
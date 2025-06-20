#pragma once

#include <iosfwd>
#include <iostream>

#include "FunctionSet.h"

class Module final {
public:
    const FunctionData* make_function_data(std::size_t id, FunctionPrototype&& proto, std::vector<ArgumentValue>&& args) {
        const auto [fst, snd] = m_functions.emplace(id, std::move(proto), std::move(args));
        return &(*fst);
    }

    void print(std::ostream& os) const;
private:
    fn::FunctionSet m_functions;
};

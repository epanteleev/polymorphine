#include "mir/module/Module.h"

#include <ranges>

const FunctionData* Module::add_function_data(FunctionPrototype &&proto, std::vector<ArgumentValue> &&args) {
    assertion(proto.arg_types().size() == args.size(),
          "Number of arguments does not match prototype m_args={}, arg_types={}", args.size(), proto.arg_types().size());

#ifndef NDEBUG
    for (auto [a, b]: std::ranges::views::zip(args, proto.arg_types())) {
        assertion(a.type() == b, "Argument type mismatch");
    }
#endif

    const auto& [fst, snd] = m_functions.emplace(proto.name(), std::make_unique<FunctionData>(std::move(proto), std::move(args)));
    return fst->second.get();
}

std::ostream & operator<<(std::ostream &os, const Module &module) {
    for (const auto &s: module.m_known_structs | std::views::values) {
        os << '$' << s->name() << " = type { ";
        for (const auto &ft: s->field_types()) {
            os << *ft << ' ';
        }
        os << "}" << std::endl;
    }

    for (const auto &f: module.m_functions | std::views::values) {
        f->print(os);
    }

    return os;
}

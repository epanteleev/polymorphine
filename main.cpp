#include <iostream>

#include "lib/ir.h"
#include "lib/pass/analysis/Analysis.h"

Module fib() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "fib");

    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    auto n = data.arg(0);
    auto ret_addr = data.alloc(SignedIntegerType::i32());
    auto n_addr = data.alloc(SignedIntegerType::i32());

    auto a = data.alloc(SignedIntegerType::i32());
    auto b = data.alloc(SignedIntegerType::i32());
    auto c = data.alloc(SignedIntegerType::i32());
    auto i = data.alloc(SignedIntegerType::i32());

    data.store(n_addr, n);
    data.store(a, Value::i32(0));
    data.store(b, Value::i32(1));

    auto v0 = data.load(n_addr);
    auto cmp0 = data.icmp(IcmpPredicate::Eq, v0, Value::i32(0));

    auto if_then = data.create_basic_block();
    auto if_end = data.create_basic_block();
    auto for_cond = data.create_basic_block();
    auto for_body = data.create_basic_block();
    auto for_end = data.create_basic_block();
    auto for_inc = data.create_basic_block();
    auto ret = data.create_basic_block();

    data.br_cond(cmp0, if_then, if_end);

    data.switch_block(if_then);

    auto v1 = data.load(b);
    data.store(ret_addr, v1);
    data.br(ret);

    data.switch_block(if_end);
    data.store(i, Value::i32(2));
    data.br(for_cond);

    data.switch_block(for_cond);
    auto v2 = data.load(n_addr);
    auto v3 = data.load(i);
    auto cmp = data.icmp(IcmpPredicate::Lt, v3, v2);
    data.br_cond(cmp, for_body, for_end);

    data.switch_block(for_body);
    auto v4 = data.load(a);
    auto v5 = data.load(b);
    auto v6 = data.add(v4, v5);
    data.store(c, v6);
    data.store(a, v5);
    data.store(b, v6);
    data.br(for_inc);

    data.switch_block(for_inc);

    auto v7 = data.load(i);
    auto v8 = data.add(v7, Value::i32(1));
    data.store(i, v8);

    data.br(for_cond);

    data.switch_block(for_end);
    auto v9 = data.load(b);
    data.store(ret_addr, v9);
    data.br(ret);

    data.switch_block(ret);
    auto v10 = data.load(ret_addr);

    data.ret(v10);

    return builder.build();
}

int main() {
    constexpr auto type = SignedIntegerType::i32();
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    FunctionPrototype proto(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "main");
    ArgumentValue arg(1, SignedIntegerType::i32());
    auto module = fib();
    const auto fd = module.find_function_data("fib").value();

    AnalysisPassCache cache;
    const auto dominator_tree = cache.analyze<DominatorTreeEval>(fd);

    std::cout << "DomTree: ";
    dominator_tree->print(std::cout) << std::endl;
    module.print(std::cout) << std::endl;

    const auto& bfs = *cache.analyze<BFSOrderTraverse>(fd);
    std::cout << "BFS Order: ";
    for (const auto bb : bfs) {
        bb->print_short_name(std::cout);
        std::cout << ' ';
    }
    return 0;
}

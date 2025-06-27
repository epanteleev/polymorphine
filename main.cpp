#include <iostream>

#include "lib/ir.h"
#include "lib/pass/analysis/Analysis.h"
#include "pass/analysis/AnalysisPassCache.h"
#include "platform/lower/Lowering.h"

Module ret_one() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    data.ret(Value::i32(1));
    return builder.build();
}


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
    auto module0 = ret_one();
    module0.print(std::cout) << std::endl;

    Lowering lower(module0);
    lower.run();
    auto result = lower.result();
    result.print(std::cout) << std::endl;

    auto module = fib();
    const auto fd = module.find_function_data("fib").value();

    AnalysisPassCache cache;

    auto start = std::chrono::high_resolution_clock::now();


    const auto loop = cache.analyze<LoopInfoEval>(fd);


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    std::cout << loop << std::endl;
    return 0;
}

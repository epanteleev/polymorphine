#include <iomanip>
#include <iostream>

#include "mir/mir.h"
#include "lir/x64/lir.h"

#include "lib/lir/x64/asm/jit/JitAssembler.h"
#include "lir/x64/codegen/Codegen.h"
#include "lir/x64/codegen/MachFunctionCodegen.h"

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

std::size_t single_thread_solution() {
    auto module = fib();
    const auto fd = module.find_function_data("fib").value();

    std::size_t sum{};

    for (int i = 0; i < 2000; i++) {
        AnalysisPassCache cache;

        auto start = std::chrono::steady_clock::now();

        auto dom_tree = cache.analyze<DominatorTreeEval>(fd);
        auto bfs = cache.analyze<BFSOrderTraverse>(fd);
        auto postorder = cache.analyze<PostOrderTraverse>(fd);

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        sum += duration.count();

        std::cout << dom_tree << std::endl;
        std::cout << bfs << std::endl;
        std::cout << postorder << std::endl;
    }

    return sum / 2000;
}

std::size_t async_based_solution() {
    auto module = fib();
    const auto fd = module.find_function_data("fib").value();

    std::size_t sum{};

    for (int i = 0; i < 2000; i++) {
        AnalysisPassCache cache;
        auto start = std::chrono::steady_clock::now();

        auto dom_tree = cache.concurrent_analyze<DominatorTreeEval>(fd);
        auto bfs = cache.concurrent_analyze<BFSOrderTraverse>(fd);
        auto postorder = cache.concurrent_analyze<PostOrderTraverse>(fd);

        const auto res = dom_tree.get();
        auto res_bfs = bfs.get();
        auto res_postorder = postorder.get();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        sum += duration.count();

        std::cout << res << std::endl;
        std::cout << res_bfs << std::endl;
        std::cout << res_postorder << std::endl;
    }

    return sum / 2000;
}

int main() {
    auto module = fib();
    const auto fd = module.find_function_data("fib").value();
    fd->print(std::cout);

    AnalysisPassCache cache0;

    auto module0 = ret_one();
    module0.print(std::cout) << std::endl;

    Lowering lower(module0);
    lower.run();
    const auto result = lower.result();
    result.print(std::cout) << std::endl;

    auto obj_fun = result.find_function_data("ret_one").value();

    AnalysisPassCacheMach cache;
    auto liveness = cache.analyze<LivenessAnalysis>(obj_fun);
    auto liveintervals = cache.analyze<LiveIntervalsEval>(obj_fun);
    std::cout << *liveintervals << std::endl;

    auto register_alloc = cache.analyze<LinearScan>(obj_fun);
    std::cout << *register_alloc << std::endl;

    Codegen codegen(result);
    codegen.run();
    const auto mach = codegen.result();

    const auto buffer = JitAssembler::assembly(mach);

    const auto fn = reinterpret_cast<int(*)()>(buffer.code_start("ret_one").value());
    const auto res = fn();
    std::cout << "Result of 'ret_one': " << res << std::endl;

    /*
    const auto time1 = async_based_solution();
    const auto time2 = single_thread_solution();

    std::cout << "'async_based_solution' average time: " << time1 << " ns"<< std::endl;
    std::cout << "'single_thread_solution' average time: " << time2 << " ns"<< std::endl;
*/
    return 0;
}

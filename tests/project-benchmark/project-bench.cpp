#include <benchmark/benchmark.h>

#include "C:\Users\ajpur\Documents\Education\University\Comp203 (Optimisations)\COMP203-2202628\src\main.cpp"

#include <benchmark/benchmark.h>

#include "labs/sorting/lab1.hpp"

static void BM_CountTimer(benchmark::State& state) {
    // Perform setup here
    for (auto _ : state) {
        
        // This code gets timed
        main();
        benchmark::ClobberMemory();
    }

    // for big oh notation
    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_CountTimer)->RangeMultiplier(2)->Range(1 << 10, 1 << 16)->Complexity();

// Run the benchmark
BENCHMARK_MAIN();

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <benchmark/benchmark.h>
#include <gxhash.h>

#include "xxhash.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

#ifdef AVX2
#include <immintrin.h>
#endif

#ifdef RUNNING_ON_ANDROID
const std::string dataset_path = "/data/local/tmp/external_dataset.txt";
const std::string lzbench_path = "lzbench"; // will be found from system PATH

#else
const std::string dataset_path = "../data/external_dataset.txt";
const std::string lzbench_path = "lzbench";
#endif


std::mutex delay_mutex;
namespace fs = std::filesystem;

std::vector<std::string> load_dataset() {
    std::vector<std::string> dataset;
    std::ifstream file(dataset_path);
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            dataset.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "[ERROR] Could not open file: " << dataset_path << "\n";
    }
    return dataset;
}

void run_lzbench() {
    std::cout << "\nRunning LZBench Compression Benchmarks...\n";

    if (access(lzbench_path.c_str(), X_OK) == -1) {
        std::cerr << "Error: LZBench binary is not executable. Trying to set permissions...\n";
        system(("chmod +x " + lzbench_path).c_str());
    }

    std::string command = lzbench_path + " -c -t1 -i1 " + dataset_path;
    int ret_code = std::system(command.c_str());

    if (ret_code != 0) {
        std::cerr << "LZBench failed to run. Please verify the path.\n";
    }
}

void run_benchmarks() {
    std::cout << "Running benchmarks...\n";
    auto data = load_dataset();
    if (data.empty()) {
        std::cerr << "[ERROR] Dataset is empty or could not be loaded!\n";
        return;
    }

    double total_xxhash_time = 0.0, total_gxhash_time = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& item : data) {
        volatile uint64_t hash = XXH64(item.c_str(), item.size(), 0);
    }
    auto end = std::chrono::high_resolution_clock::now();
    total_xxhash_time = std::chrono::duration<double>(end - start).count();

    std::cout << "\nXXHash took " << total_xxhash_time << " seconds.\n";

    start = std::chrono::high_resolution_clock::now();
    for (const auto& item : data) {
        volatile uint64_t hash = GXHash(item, 0);
    }
    end = std::chrono::high_resolution_clock::now();
    total_gxhash_time = std::chrono::duration<double>(end - start).count();

    std::cout << "\nGXHash took " << total_gxhash_time << " seconds.\n";

    std::cout << "\nBlock Size Specific Performance:\n";
    std::cout << "+---------------+------------------+------------------+-------------------------------+\n";
    std::cout << "| Block Size    | XXHash Time (ns) | GXHash Time (ns) | Preferred Hash Algorithm      |\n";
    std::cout << "+---------------+------------------+------------------+-------------------------------+\n";

    std::vector<size_t> block_sizes = {64, 512, 4096, 2 * 1024 * 1024};

    for (size_t block_size : block_sizes) {
        std::string block(block_size, 'A');

        auto start = std::chrono::high_resolution_clock::now();
        volatile uint64_t h1 = XXH64(block.c_str(), block.size(), 0);
        auto end = std::chrono::high_resolution_clock::now();
        double xx_time = std::chrono::duration<double, std::nano>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        volatile uint64_t h2 = GXHash(block, 0);
        end = std::chrono::high_resolution_clock::now();
        double gx_time = std::chrono::duration<double, std::nano>(end - start).count();

        std::string preferred = (gx_time < xx_time) ? "GXHash" : "XXHash";

        std::ostringstream size_str;
        if (block_size >= 1024 * 1024)
            size_str << (block_size / (1024 * 1024)) << " MB";
        else if (block_size >= 1024)
            size_str << (block_size / 1024) << " KB";
        else
            size_str << block_size << " B";

        std::cout << "| " << std::setw(12) << size_str.str()
                  << " | " << std::setw(16) << std::fixed << std::setprecision(2) << xx_time
                  << " | " << std::setw(16) << gx_time
                  << " | " << std::setw(29) << preferred << " |\n";
    }
    std::cout << "+---------------+------------------+------------------+-------------------------------+\n";

    run_lzbench();

    std::cout << "\nRunning Google Benchmarks...\n";
    int argc = 1;
    char arg0[] = "benchmark";
    char* argv[] = {arg0, nullptr};
    benchmark::Initialize(&argc, argv);
    if (!benchmark::ReportUnrecognizedArguments(argc, argv))
        benchmark::RunSpecifiedBenchmarks();
}

// ------------------- GXHash Benchmarks -----------------------
static void BM_GXHash_64B(benchmark::State& state) {
    std::string data(64, 'x');
    for (auto _ : state) {
        volatile auto h = GXHash(data, 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_GXHash_64B)->Unit(benchmark::kNanosecond);

static void BM_GXHash_512B(benchmark::State& state) {
    std::string data(512, 'x');
    for (auto _ : state) {
        volatile auto h = GXHash(data, 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_GXHash_512B)->Unit(benchmark::kNanosecond);

static void BM_GXHash_4KB(benchmark::State& state) {
    std::string data(4096, 'x');
    for (auto _ : state) {
        volatile auto h = GXHash(data, 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_GXHash_4KB)->Unit(benchmark::kNanosecond);

static void BM_GXHash_2MB(benchmark::State& state) {
    std::string data(2 * 1024 * 1024, 'x');
    for (auto _ : state) {
        volatile auto h = GXHash(data, 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_GXHash_2MB)->Unit(benchmark::kMicrosecond);

// ------------------- XXHash Benchmarks -----------------------
static void BM_XXHash_64B(benchmark::State& state) {
    std::string data(64, 'x');
    for (auto _ : state) {
        volatile auto h = XXH64(data.c_str(), data.size(), 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_XXHash_64B)->Unit(benchmark::kNanosecond);

static void BM_XXHash_512B(benchmark::State& state) {
    std::string data(512, 'x');
    for (auto _ : state) {
        volatile auto h = XXH64(data.c_str(), data.size(), 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_XXHash_512B)->Unit(benchmark::kNanosecond);

static void BM_XXHash_4KB(benchmark::State& state) {
    std::string data(4096, 'x');
    for (auto _ : state) {
        volatile auto h = XXH64(data.c_str(), data.size(), 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_XXHash_4KB)->Unit(benchmark::kNanosecond);

static void BM_XXHash_2MB(benchmark::State& state) {
    std::string data(2 * 1024 * 1024, 'x');
    for (auto _ : state) {
        volatile auto h = XXH64(data.c_str(), data.size(), 0);
        benchmark::DoNotOptimize(h);
    }
}
BENCHMARK(BM_XXHash_2MB)->Unit(benchmark::kMicrosecond);

// ------------------- Main Function ---------------------
int main() {
    std::cout << "Initializing benchmarks...\n";
    run_benchmarks();
    return 0;
}
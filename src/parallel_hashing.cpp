#include "parallel_hashing.h"
#include "xxhash.h"
#include <thread>
#include <mutex>
#include <vector>
#include <iostream>

// Mutex for safely updating global hash
std::mutex result_mutex;

// Function to hash a chunk of data using thread-local storage
void hash_chunk(const std::vector<std::string>& data, size_t start, size_t end, unsigned int& global_result) {
    unsigned int local_result = 0;

    for (size_t i = start; i < end; ++i) {
        local_result ^= XXH32(data[i].c_str(), data[i].size(), 0);
    }

    // Update global result safely
    {
        std::lock_guard<std::mutex> lock(result_mutex);
        global_result ^= local_result;
    }
}

// Function to perform parallel hashing
void parallel_hash(const std::vector<std::string>& dataset, unsigned int& result) {
    size_t num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback to 4 threads if hardware concurrency is unavailable

    size_t chunk_size = dataset.size() / num_threads;
    if (chunk_size == 0) chunk_size = 1; // Ensure each thread gets at least one element

    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == num_threads - 1) ? dataset.size() : start + chunk_size;

        threads.emplace_back(hash_chunk, std::cref(dataset), start, end, std::ref(result));
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// Test parallel hashing with the dataset
void test_parallel_hashing(const std::vector<std::string>& dataset) {
    unsigned int result = 0;

    parallel_hash(dataset, result);

    std::cout << "Parallel Hash Result: " << result << std::endl;
}

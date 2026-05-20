#include "environment_setup.h"
#include "deduplication.h"
#include "ksm_simulation.h"
#include "benchmark.h"
#include "parallel_hashing.h"
#include "dataset_loader.h"
#include "generate_dataset.h"
#include "macros.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>

int main()
{
#ifdef RUNNING_ON_ANDROID
    std::string dataset_path = "/data/local/tmp/external_dataset.txt";
    std::string deduplicated_output_path = "/data/local/tmp/deduplicated_output.txt";
#else
    std::string dataset_path = "../data/external_dataset.txt";
    std::string deduplicated_output_path = "../data/deduplicated_output.txt";
#endif


    int num_entries = 100000; // Required for generate_dataset

    // Step 1: Environment Setup
    std::cout << "Setting up environment..." << std::endl;
    setup_environment();

    // Step 2: Ensure Dataset Exists
    std::ifstream infile(dataset_path);
    if (!infile.good())
    {
        std::cout << "Generating dataset..." << std::endl;
        generate_dataset(dataset_path); 
    }
    infile.close();

    // Step 3: Load Dataset
    std::cout << "Loading dataset from " << dataset_path << "..." << std::endl;
    auto dataset = load_dataset(dataset_path);

    if (dataset.empty())
    {
        std::cerr << "[ERROR] Dataset is empty or could not be loaded!" << std::endl;
        return 1;
    }

    // Step 4: Deduplication
    std::cout << "Performing deduplication..." << std::endl;
    auto [deduplicated_data, duplicate_count, memory_saved] = deduplicate_and_count(dataset);

    save_deduplicated_data(deduplicated_output_path, deduplicated_data);

    // Step 5: Display Deduplication Statistics
    std::cout << "\n===== Deduplication Statistics =====" << std::endl;
    std::cout << "Total Pages: " << dataset.size() << std::endl;
    std::cout << "Unique Pages: " << deduplicated_data.size() << std::endl;
    std::cout << "Duplicate Pages: " << duplicate_count << std::endl;
    if (memory_saved < 1024)
        std::cout << "Memory Saved: " << memory_saved << " bytes" << std::endl;
    else
        std::cout << "Memory Saved: " << (memory_saved / 1024) << " KB" << std::endl;

    // Step 6: Simulate Kernel Same-page Merging (KSM)
    std::cout << "Simulating Kernel Same-page Merging (KSM)..." << std::endl;
    simulate_ksm(deduplicated_data);

    // Step 8: Test Parallel Hashing
    std::cout << "Testing parallel hashing..." << std::endl;
    test_parallel_hashing(deduplicated_data);

    std::cout << "\nIntegration complete. System is ready for use." << std::endl;
    return 0;
}

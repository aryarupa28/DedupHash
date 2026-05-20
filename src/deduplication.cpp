#include "deduplication.h"
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <tuple>
#include <vector>

// Function to deduplicate the dataset and return duplicate count and memory saved
std::tuple<std::vector<std::string>, int, size_t> deduplicate_and_count(const std::vector<std::string>& dataset) {
    std::unordered_set<std::string> unique_data;
    std::vector<std::string> deduplicated_data;
    int duplicate_count = 0;
    size_t memory_saved = 0;

    // Iterate through the dataset and add to the set
    for (const auto& data : dataset) {
        if (unique_data.insert(data).second) {  // If insertion is successful (i.e., unique entry)
            deduplicated_data.push_back(data);
        } else {
            duplicate_count++;
            memory_saved += data.size();  // Track memory saved
        }
    }

    std::cout << "Deduplication completed. Unique Entries: " << deduplicated_data.size()
              << ", Duplicates Found: " << duplicate_count;
    if (memory_saved < 1024)
        {
            std::cout << ", Memory Saved: " << memory_saved << " bytes" << std::endl;
        }
    else
        {
            std::cout << ", Memory Saved: " << (memory_saved / 1024) << " KB" << std::endl;
        }
          

    return {deduplicated_data, duplicate_count, memory_saved};
}

// Function to save the deduplicated data to a file
void save_deduplicated_data(const std::string& output_file, const std::vector<std::string>& deduplicated_data) {
    std::ofstream out_file(output_file);

    if (!out_file.is_open()) {
        std::cerr << "[ERROR] Error opening file for writing: " << output_file << std::endl;
        return;
    }

    for (const auto& data : deduplicated_data) {
        out_file << data << "\n";
    }
    
    out_file.close();
    std::cout << "Deduplicated data saved to: " << output_file << std::endl;
}

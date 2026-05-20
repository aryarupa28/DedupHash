#ifndef XXHASH_INTEGRATION_H
#define XXHASH_INTEGRATION_H

#include "platform_simd.h"  // Architecture-aware SIMD intrinsics

#include <vector>
#include <string>

// Declare the function prototypes
void hash_data(const std::vector<std::string>& data);
void simd_optimized_hash(const std::vector<std::string>& data);
void multithreaded_hash(const std::vector<std::string>& data);

#endif // XXHASH_INTEGRATION_H

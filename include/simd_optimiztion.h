#ifndef SIMD_OPTIMIZATION_H
#define SIMD_OPTIMIZATION_H

#include "platform_simd.h"  // Add this to bring in the right SIMD intrinsics for x86 or ARM

#include <vector>
#include <string>

void simd_optimized_hash(const std::vector<std::string>& data);

#endif // SIMD_OPTIMIZATION_H

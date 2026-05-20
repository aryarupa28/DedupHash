#include <vector>
#include <string>
#include <xxhash.h>

#include <gxhash.h>


#include "debug_log.hpp"  // For conditional debug logging

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
  #ifdef AVX2
    #include <immintrin.h>
  #endif
#elif defined(__aarch64__) || defined(__arm__)
  #include <arm_neon.h>
#endif

// SIMD-optimized hash function supporting AVX2 (x86) and NEON (ARM)
void simd_optimized_hash(const std::vector<std::string>& data) {
    DEBUG_LOG("Using SIMD-optimized hashing...");

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
  #ifdef AVX2
    DEBUG_LOG("AVX2 optimization is enabled.");

    for (const auto& element : data) {
        size_t length = element.size();
        const char* input = element.c_str();

        unsigned int hash = 0;
        size_t chunks = length / 32;  // Each AVX2 256-bit register handles 32 bytes

        DEBUG_LOG("Processing element of length " << length << "...");

        // Process full 32-byte chunks
        for (size_t i = 0; i < chunks; ++i) {
            __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input + i * 32));
            unsigned int chunk_hash = XXH32(reinterpret_cast<const char*>(&chunk), 32, 0);
            hash ^= chunk_hash;

            DEBUG_LOG("Processing chunk " << i << " (32 bytes), Hash: " << chunk_hash);
        }

        // Process remaining bytes
        size_t remaining = length % 32;
        if (remaining > 0) {
            unsigned int remaining_hash = XXH32(input + chunks * 32, remaining, 0);
            hash ^= remaining_hash;

            DEBUG_LOG("Processing remaining " << remaining << " bytes, Hash: " << remaining_hash);
        }

        DEBUG_LOG("Final hash for element: " << hash);
    }
  #else
    DEBUG_LOG("AVX2 not enabled, using fallback hashing.");
    for (const auto& element : data) {
        unsigned int hash = XXH32(element.c_str(), element.size(), 0);
        DEBUG_LOG("Hash for element: " << hash);
    }
  #endif

#elif defined(__aarch64__) || defined(__arm__)
  #ifdef ARM_NEON
    DEBUG_LOG("ARM NEON optimization is enabled.");

    for (const auto& element : data) {
        size_t length = element.size();
        const char* input = element.c_str();

        unsigned int hash = 0;
        size_t chunks = length / 16;  // NEON 128-bit registers handle 16 bytes

        DEBUG_LOG("Processing element of length " << length << "...");

        // Process full 16-byte chunks with NEON
        for (size_t i = 0; i < chunks; ++i) {
            uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(input + i * 16));
            // Convert NEON register to char array for XXH32 hashing
            alignas(16) unsigned char buffer[16];
            vst1q_u8(buffer, chunk);

            unsigned int chunk_hash = XXH32(reinterpret_cast<const char*>(buffer), 16, 0);
            hash ^= chunk_hash;

            DEBUG_LOG("Processing chunk " << i << " (16 bytes), Hash: " << chunk_hash);
        }

        // Process remaining bytes
        size_t remaining = length % 16;
        if (remaining > 0) {
            unsigned int remaining_hash = XXH32(input + chunks * 16, remaining, 0);
            hash ^= remaining_hash;

            DEBUG_LOG("Processing remaining " << remaining << " bytes, Hash: " << remaining_hash);
        }

        DEBUG_LOG("Final hash for element: " << hash);
    }
  #else
    DEBUG_LOG("ARM NEON not enabled, using fallback hashing.");
    for (const auto& element : data) {
        unsigned int hash = XXH32(element.c_str(), element.size(), 0);
        DEBUG_LOG("Hash for element: " << hash);
    }
  #endif

#else
    DEBUG_LOG("SIMD optimizations not available. Using fallback hashing.");

    for (const auto& element : data) {
        unsigned int hash = XXH32(element.c_str(), element.size(), 0);
        DEBUG_LOG("Hash for element: " << hash);
    }
#endif

    DEBUG_LOG("SIMD optimized hashing completed for " << data.size() << " elements.");
}

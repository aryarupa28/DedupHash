#include "gxhash.h"

uint32_t GXHash(const void* data, size_t length, uint32_t seed) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t hash = seed ? seed : 2166136261u;  // FNV offset basis

    for (size_t i = 0; i < length; ++i) {
        hash ^= bytes[i];
        hash *= 16777619u;  // FNV prime
    }

    return hash;
}

uint32_t GXHash(const std::string& str, uint32_t seed) {
    return GXHash(str.data(), str.size(), seed);
}

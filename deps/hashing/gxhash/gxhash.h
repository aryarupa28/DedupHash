#ifndef GXHASH_H
#define GXHASH_H

#include <string>
#include <cstddef>
#include <cstdint>

// Core hashing function (for binary data)
uint32_t GXHash(const void* data, size_t length, uint32_t seed = 0);

// Convenience function (for std::string)
uint32_t GXHash(const std::string& str, uint32_t seed = 0);

#endif // GXHASH_H

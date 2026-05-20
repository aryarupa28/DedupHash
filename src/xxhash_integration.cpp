#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <xxhash.h>


#if defined(__x86_64__)
    #include <immintrin.h> // For AVX2/AVX-512 intrinsics
    #define SIMD_X86
#elif defined(__aarch64__)
    #include <arm_neon.h> // For NEON intrinsics
    #define SIMD_ARM
#endif

void computeHashScalar(const std::string& input) {
    unsigned int hash = XXH32(input.c_str(), input.size(), 0);
    std::cout << "Scalar Hash of input: " << hash << std::endl;
}

#if defined(SIMD_X86)
void computeHashSIMD(const std::string& input) {
    const char* data = input.c_str();
    size_t size = input.size();
    size_t chunkSize = 32;
    unsigned int hash = 0;

    for (size_t i = 0; i < size; i += chunkSize) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
        char buffer[32];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(buffer), chunk);
        hash ^= XXH32(buffer, chunkSize, 0);
    }

    std::cout << "AVX2 Hash of input: " << hash << std::endl;
}
#elif defined(SIMD_ARM)
void computeHashSIMD(const std::string& input) {
    const char* data = input.c_str();
    size_t size = input.size();
    size_t chunkSize = 16;
    unsigned int hash = 0;

    for (size_t i = 0; i < size; i += chunkSize) {
        uint8x16_t chunk = vld1q_u8(reinterpret_cast<const uint8_t*>(data + i));
        char buffer[16];
        memcpy(buffer, &chunk, sizeof(chunk));
        hash ^= XXH32(buffer, chunkSize, 0);
    }

    std::cout << "NEON Hash of input: " << hash << std::endl;
}
#endif

void computeHash(const std::string& input) {
#if defined(SIMD_X86) || defined(SIMD_ARM)
    computeHashSIMD(input);
#else
    computeHashScalar(input);
#endif
}

int test_xxhash_integration(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <string|file> [input]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    if (mode == "string") {
        computeHash(argv[2]);
    } else if (mode == "file") {
        std::ifstream file(argv[2], std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << argv[2] << std::endl;
            return 1;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size)) {
            computeHash(std::string(buffer.begin(), buffer.end()));
        } else {
            std::cerr << "Failed to read file: " << argv[2] << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Invalid arguments. Usage: " << argv[0] << " <string|file> [input]" << std::endl;
        return 1;
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <gxhash.h>


#if defined(__x86_64__)
    #include <immintrin.h>
    #define SIMD_X86
#elif defined(__aarch64__)
    #include <arm_neon.h>
    #define SIMD_ARM
#endif

void computeHashScalar(const std::string& input) {
    uint32_t hash = GXHash(input);  // Directly pass the string
    std::cout << "Scalar GXHash of input: " << hash << std::endl;
}

#if defined(SIMD_X86)
void computeHashSIMD(const std::string& input) {
    const char* data = input.c_str();
    size_t size = input.size();
    size_t chunkSize = 32;
    uint32_t hash = 0;

    for (size_t i = 0; i < size; i += chunkSize) {
        size_t actualSize = std::min(chunkSize, size - i);
        char buffer[32] = {0};
        memcpy(buffer, data + i, actualSize);
        hash ^= GXHash(buffer, actualSize, 0);
    }

    std::cout << "AVX2 GXHash of input: " << hash << std::endl;
}
#elif defined(SIMD_ARM)
void computeHashSIMD(const std::string& input) {
    const char* data = input.c_str();
    size_t size = input.size();
    size_t chunkSize = 16;
    uint32_t hash = 0;

    for (size_t i = 0; i < size; i += chunkSize) {
        size_t actualSize = std::min(chunkSize, size - i);
        char buffer[16] = {0};
        memcpy(buffer, data + i, actualSize);
        hash ^= GXHash(buffer, actualSize, 0);
    }

    std::cout << "NEON GXHash of input: " << hash << std::endl;
}
#endif

void computeHash(const std::string& input) {
#if defined(SIMD_X86) || defined(SIMD_ARM)
    computeHashSIMD(input);
#else
    computeHashScalar(input);
#endif
}

int test_gxhash_integration(int argc, char* argv[]) {
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

// Entry point
int main(int argc, char* argv[]) {
    return test_gxhash_integration(argc, argv);
}

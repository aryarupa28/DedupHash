#include <gtest/gtest.h>
#include <gxhash.h>



TEST(HashingTests, TestSimpleInput) {
    std::string input = "hello";
    uint64_t hash1 = GXHash(input.data(), input.size());
    uint64_t hash2 = GXHash(input.data(), input.size());
    EXPECT_EQ(hash1, hash2) << "Hash function is not deterministic!";
}

TEST(HashingTests, TestEmptyInput) {
    std::string input = "";
    uint64_t hash1 = GXHash(input.data(), input.size());
    uint64_t expected_empty_hash = GXHash("", 0);  // Ensure consistency
    EXPECT_EQ(hash1, expected_empty_hash) << "Empty input should always return the same hash!";
}

TEST(HashingTests, TestLargeInput) {
    std::string input(1024 * 1024, 'A');  // 1MB input
    uint64_t hash1 = GXHash(input.data(), input.size());
    uint64_t hash2 = GXHash(input.data(), input.size());
    EXPECT_EQ(hash1, hash2) << "Large input hashing failed!";
}

TEST(HashingTests, TestHashCollisions) {
    std::string input1 = "apple";
    std::string input2 = "applf";  // Minor change
    uint64_t hash1 = GXHash(input1.data(), input1.size());
    uint64_t hash2 = GXHash(input2.data(), input2.size());
    
    EXPECT_NE(hash1, hash2) << "Collision detected: Different inputs have the same hash!\n"
                            << "Hash1: " << hash1 << "\n"
                            << "Hash2: " << hash2;
}

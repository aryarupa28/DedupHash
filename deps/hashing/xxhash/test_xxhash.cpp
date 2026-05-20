#include <iostream>
#include <cstring>
#include "xxhash.h"

int main() {
    const char* input = "Hello, world!";
    unsigned int hash = XXH32(input, strlen(input), 0);
    
    std::cout << "xxHash32: " << hash << std::endl;
    return 0;
}

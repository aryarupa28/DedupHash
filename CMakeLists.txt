cmake_minimum_required(VERSION 3.10)
project(hashing_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Detect Android or processor type
if(CMAKE_SYSTEM_NAME STREQUAL "Android" OR ANDROID)
    set(RUNNING_ON_ANDROID TRUE)
    set(TARGET_ARCH "ARM64")
else()
    set(RUNNING_ON_ANDROID FALSE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(TARGET_ARCH "ARM64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        set(TARGET_ARCH "X86_64")
    else()
        set(TARGET_ARCH "UNKNOWN")
    endif()
endif()

message(STATUS "Target architecture: ${TARGET_ARCH}")

# Strip -mfpu if present (invalid on AArch64)
if(TARGET_ARCH STREQUAL "ARM64")
    foreach(flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
                    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        if(DEFINED ${flag_var})
            string(REGEX REPLACE "-mfpu=[^ ]+" "" TMP "${${flag_var}}")
            set(${flag_var} "${TMP}" CACHE STRING "Stripped -mfpu" FORCE)
        endif()
    endforeach()
endif()

# Optimization flags
include(CheckCXXCompilerFlag)
if(TARGET_ARCH STREQUAL "X86_64")
    check_cxx_compiler_flag("-mavx2" HAS_AVX2)
    if(HAS_AVX2)
        message(STATUS "AVX2 supported: enabling -mavx2 and AVX2 macro")
        add_compile_options(-march=native -mavx2 -O2)
        add_compile_definitions(AVX2)
    else()
        add_compile_options(-O2)
    endif()
elseif(TARGET_ARCH STREQUAL "ARM64")
    check_cxx_compiler_flag("-march=armv8-a+crypto+simd" HAS_ARM_FEATURES)
    if(HAS_ARM_FEATURES)
        add_compile_options(-O2 -ftree-vectorize -march=armv8-a+crypto+simd)
        add_compile_definitions(ARM_NEON AES_NEON_OPT)
    else()
        add_compile_options(-O2)
        add_compile_definitions(ARM_NEON)
    endif()
else()
    add_compile_options(-O2)
endif()

# Android macro
if(RUNNING_ON_ANDROID)
    add_definitions(-DRUNNING_ON_ANDROID)
endif()

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/deps
    ${PROJECT_SOURCE_DIR}/deps/hashing/xxhash
    ${PROJECT_SOURCE_DIR}/deps/hashing/gxhash
)

# Source files
set(SOURCES
    src/generate_dataset.cpp
    src/deduplication.cpp
    src/parallel_hashing.cpp
    src/simd_optimization.cpp
    src/xxhash_integration.cpp
    src/dataset_loader.cpp
    src/environment_setup.cpp
    src/ksm_simulation.cpp
    src/gxhash_integration.cpp
    deps/hashing/gxhash/gxhash.cpp
)

# xxHash static library
add_library(xxhash STATIC deps/hashing/xxhash/xxhash.c)

# Core hashing library
add_library(hashing_lib STATIC ${SOURCES})
target_include_directories(hashing_lib PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/deps)

if(RUNNING_ON_ANDROID)
    target_link_libraries(hashing_lib PRIVATE xxhash)
else()
    target_link_libraries(hashing_lib PRIVATE xxhash pthread)
endif()

# Google Benchmark (non-Android)
if(NOT RUNNING_ON_ANDROID)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    set(BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(deps/testing/benchmark)
    set(USE_GOOGLE_BENCHMARK TRUE)
else()
    set(USE_GOOGLE_BENCHMARK FALSE)
endif()

# GoogleTest
add_subdirectory(deps/testing/googletest)

# Main binary
add_executable(hashing_system src/main.cpp)
target_link_libraries(hashing_system PRIVATE hashing_lib)
if(NOT RUNNING_ON_ANDROID)
    target_link_libraries(hashing_system PRIVATE pthread)
endif()

# GXHash integration binary
add_executable(gxhash_integration src/gxhash_integration.cpp)
target_link_libraries(gxhash_integration PRIVATE hashing_lib)
if(NOT RUNNING_ON_ANDROID)
    target_link_libraries(gxhash_integration PRIVATE pthread)
endif()

# Unit tests
enable_testing()
add_executable(tests tests/test_main.cpp tests/test_hashing.cpp)
target_link_libraries(tests PRIVATE gtest gtest_main hashing_lib)
if(NOT RUNNING_ON_ANDROID)
    target_link_libraries(tests PRIVATE pthread)
endif()
add_test(NAME hashing_tests COMMAND tests)

# Benchmark binary
if(USE_GOOGLE_BENCHMARK)
    add_executable(benchmarks tests/benchmark.cpp)
    target_link_libraries(benchmarks PRIVATE benchmark::benchmark hashing_lib pthread)
endif()

# Summary
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")

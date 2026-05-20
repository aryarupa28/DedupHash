#ifndef MACROS_H
#define MACROS_H

#include <iostream>
#include <iomanip>

// Debug compilation control
#ifndef DEBUG_ENABLED
    #ifdef NDEBUG
        #define DEBUG_ENABLED 0
    #else
        #define DEBUG_ENABLED 1
    #endif
#endif

// Performance debug control (can be disabled even in debug builds for benchmarks)
#ifndef PERF_DEBUG_ENABLED
    #ifdef BENCHMARK_BUILD
        #define PERF_DEBUG_ENABLED 0
    #else
        #define PERF_DEBUG_ENABLED DEBUG_ENABLED
    #endif
#endif

// Base debug macro that completely disappears when disabled
#if DEBUG_ENABLED
    #define DEBUG_PRINT(tag, message) \
        do { \
            std::cout << "[" << tag << "] " << message << std::endl; \
        } while(0)
#else
    #define DEBUG_PRINT(tag, message) ((void)0)
#endif

// Performance-sensitive debug macro
#if PERF_DEBUG_ENABLED
    #define PERF_DEBUG_PRINT(tag, message) DEBUG_PRINT(tag, message)
#else
    #define PERF_DEBUG_PRINT(tag, message) ((void)0)
#endif

// Specific debug level macros matching your existing patterns
#define DEBUG_LOG(message)     DEBUG_PRINT("DEBUG", message)
#define ERROR_LOG(message)     DEBUG_PRINT("ERROR", message)
#define WARNING_LOG(message)   DEBUG_PRINT("WARNING", message)
#define SUCCESS_LOG(message)   DEBUG_PRINT("SUCCESS", message)

// Performance-critical debug logs (disappear in benchmark builds)
#define PERF_DEBUG_LOG(message) PERF_DEBUG_PRINT("DEBUG", message)

// Debug blocks for complex debugging code
#if DEBUG_ENABLED
    #define DEBUG_BLOCK if (true)
#else
    #define DEBUG_BLOCK if (false)
#endif

#if PERF_DEBUG_ENABLED
    #define PERF_DEBUG_BLOCK if (true)
#else
    #define PERF_DEBUG_BLOCK if (false)
#endif

// Function tracing macros
#define DEBUG_FUNCTION_ENTER() DEBUG_LOG("Entering " << __FUNCTION__)
#define DEBUG_FUNCTION_EXIT()  DEBUG_LOG("Exiting " << __FUNCTION__)

// Memory and performance related debug macros
#define DEBUG_MEMORY(message)    PERF_DEBUG_PRINT("MEMORY", message)
#define DEBUG_IO(message)        PERF_DEBUG_PRINT("IO", message)
#define DEBUG_PROCESS(message)   DEBUG_PRINT("PROCESS", message)

// Conditional error handling that remains even in release builds
#define ALWAYS_LOG_ERROR(message) \
    do { \
        std::cerr << "[ERROR] " << message << std::endl; \
    } while(0)

#define ALWAYS_LOG_WARNING(message) \
    do { \
        std::cerr << "[WARNING] " << message << std::endl; \
    } while(0)

#endif // MACROS_H
#ifndef DEBUG_LOG_HPP
#define DEBUG_LOG_HPP

#ifdef DEBUG_MODE
#include <iostream>
#define DEBUG_LOG(x) std::cout << x << std::endl
#else
#define DEBUG_LOG(x)
#endif

#endif // DEBUG_LOG_HPP

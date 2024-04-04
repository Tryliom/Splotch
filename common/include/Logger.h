#pragma once

#include <iostream>

#define LOG(...) std::cout << __VA_ARGS__ << "\n"
#define LOG_ERROR(...) std::cout << __VA_ARGS__ << " from " << __FILE__ << ":" << __LINE__ << "\n"
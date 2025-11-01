#ifndef POLARIS_MATH_COMMON_HPP
#define POLARIS_MATH_COMMON_HPP

#include <cmath>
#include <cstdlib>
#include <limits>
#include <numbers>
#include <random>

namespace polaris::math {
template<typename T>
concept NumericType = std::is_arithmetic_v<T>;

constexpr auto kInfinity = std::numeric_limits<double>::infinity();

inline double DegreesToRadians(double degrees) {
  return degrees * std::numbers::pi / 180.0;
}

inline double RadiansToDegrees(double radians) {
  return radians * 180.0 / std::numbers::pi;
}

template<NumericType T>
inline T RandomValue(T min = 0.0, T max = 1.0) {
  static thread_local std::mt19937 generator(std::random_device{}());

  if constexpr (std::floating_point<T>) {
    std::uniform_real_distribution<T> distribution(min, max);
    return distribution(generator);
  } else if constexpr (std::integral<T>) {
    std::uniform_int_distribution<T> distribution(min, max);
    return distribution(generator);
  }
}

inline double RandomDouble() {
  return RandomValue<double>();
}

inline double RandomDouble(double min, double max) {
  return RandomValue<double>(min, max);
}

inline int RandomInt(int min, int max) {
  return RandomValue<int>(min, max);
}
}  // namespace polaris::math

#endif

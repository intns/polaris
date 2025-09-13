#ifndef POLARIS_MATH_COMMON_HPP
#define POLARIS_MATH_COMMON_HPP

#include <cmath>
#include <cstdlib>
#include <limits>
#include <random>

namespace polaris::math {

const double kInfinity = std::numeric_limits<double>::infinity();
const double kPi = 3.1415926535897932385;

inline double DegreesToRadians(double degrees) { return degrees * kPi / 180.0; }
inline double RadiansToDegrees(double radians) { return radians * 180.0 / kPi; }
inline double RandomDouble() {
  static std::mt19937 generator(std::random_device{}());
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  return distribution(generator);
}

inline double RandomDouble(double min, double max) {
  static std::mt19937 generator(std::random_device{}());
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(generator);
}

}  // namespace polaris::math

#endif

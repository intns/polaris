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
  static std::random_device rd; // this does the job for now
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator(rd());
  return distribution(generator);
}
inline double RandomDouble(double min, double max) { return (max - min) * RandomDouble(); }

}  // namespace polaris::math

#endif

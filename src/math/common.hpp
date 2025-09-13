#ifndef POLARIS_MATH_COMMON_HPP
#define POLARIS_MATH_COMMON_HPP

#include <cmath>
#include <limits>

namespace polaris::math {

const double kInfinity = std::numeric_limits<double>::infinity();
const double kPi = 3.1415926535897932385;

inline double DegreesToRadians(double degrees) { return degrees * kPi / 180.0; }
inline double RadiansToDegrees(double radians) { return radians * 180.0 / kPi; }

}  // namespace polaris::math

#endif

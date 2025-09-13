#ifndef POLARIS_MATH_INTERVAL_HPP
#define POLARIS_MATH_INTERVAL_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>

namespace polaris::math {

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <typename T>
  requires Numeric<T>
class Interval {
  using TypeLimits = std::numeric_limits<T>;

 public:
  constexpr Interval() noexcept
      : min_(TypeLimits::max()), max_(TypeLimits::lowest()) {}

  constexpr Interval(T min_val, T max_val) noexcept
      : min_(min_val), max_(max_val) {}

  [[nodiscard]] constexpr T Min() const noexcept { return min_; }
  [[nodiscard]] constexpr T Max() const noexcept { return max_; }

  [[nodiscard]] constexpr double Size() const noexcept { return max_ - min_; }

  constexpr Interval& Expand(T delta) noexcept {
    min_ -= delta;
    max_ += delta;
    return *this;
  }

  [[nodiscard]] constexpr bool Surrounds(T x) const noexcept {
    return min_ < x && x < max_;
  }

  [[nodiscard]] constexpr double Clamp(T x) const noexcept {
    if (x < min_) return min_;
    if (x > max_) return max_;
    return x;
  }

  [[nodiscard]] constexpr bool Contains(T x) const noexcept {
    return min_ <= x && x <= max_;
  }

  [[nodiscard]] constexpr bool Overlaps(const Interval& other) const noexcept {
    return min_ <= other.max_ && other.min_ <= max_;
  }

  [[nodiscard]] static constexpr Interval Infinite() noexcept {
    if constexpr (std::floating_point<T>) {
      return Interval{-TypeLimits::infinity(), TypeLimits::infinity()};
    } else {
      return Interval{TypeLimits::lowest(), TypeLimits::max()};
    }
  }

 private:
  T min_, max_;
};

using Interval_f = Interval<float>;
using Interval_d = Interval<double>;
using Interval_i = Interval<int>;
using Interval_i64 = Interval<std::int64_t>;

}  // namespace polaris::math

#endif

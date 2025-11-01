#ifndef POLARIS_MATH_INTERVAL_HPP
#define POLARIS_MATH_INTERVAL_HPP

#include <limits>

namespace polaris::math {

class Interval {
 public:
  constexpr Interval() noexcept
      : min_(std::numeric_limits<double>::max()),
        max_(std::numeric_limits<double>::lowest()) {}

  constexpr Interval(double min_val, double max_val) noexcept
      : min_(min_val), max_(max_val) {}

  [[nodiscard]] constexpr double Min() const { return min_; }
  [[nodiscard]] constexpr double Max() const { return max_; }

  void SetMin(double min_val) { min_ = min_val; }
  void SetMax(double max_val) { max_ = max_val; }

  [[nodiscard]] constexpr double Size() const noexcept { return max_ - min_; }

  constexpr Interval Expand(double delta) const noexcept {
    auto padding = delta / 2;
    return {min_ - padding, max_ + padding};
  }

  [[nodiscard]] constexpr bool Surrounds(double X) const noexcept {
    return min_ < X && X < max_;
  }

  [[nodiscard]] constexpr double Clamp(double X) const noexcept {
    if (X < min_) return min_;
    if (X > max_) return max_;
    return X;
  }

  [[nodiscard]] constexpr bool Contains(double X) const noexcept {
    return min_ <= X && X <= max_;
  }

  [[nodiscard]] constexpr bool Overlaps(const Interval& other) const noexcept {
    return min_ <= other.max_ && other.min_ <= max_;
  }

  [[nodiscard]] static constexpr Interval Infinite() noexcept {
    return Interval{-std::numeric_limits<double>::infinity(),
                    std::numeric_limits<double>::infinity()};
  }

 private:
  double min_, max_;
};

}  // namespace polaris::math

#endif

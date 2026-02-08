
#ifndef HYPERLOGLOG_H
#define HYPERLOGLOG_H

#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

class HyperLogLog {
public:
  explicit HyperLogLog(uint8_t B)
      : B_(B), m_(1u << B), regs_(m_, 0) {}

  void add(uint32_t h) {
    uint32_t j = h >> (32 - B_);

    uint32_t w = h << B_;

    uint8_t r = rho_(w);
    if (r > regs_[j]) regs_[j] = r;
  }

  double estimate() const {
    double inv_sum = 0.0;
    uint32_t V = 0;

    for (uint8_t v : regs_) {
      inv_sum += std::ldexp(1.0, -int(v));
      if (v == 0) ++V;
    }

    double m = double(m_);
    double alpha = 0.7213 / (1.0 + 1.079 / m);
    double E = alpha * m * m / inv_sum;

    if (V > 0 && E <= 2.5 * m) {
      E = m * std::log(m / double(V));
    }

    return E;
  }

private:
  uint8_t B_;
  uint32_t m_;
  std::vector<uint8_t> regs_;

  static uint8_t rho_(uint32_t x) {
    if (x == 0) return 32;
#if defined(__GNUG__) || defined(__clang__)
    return uint8_t(__builtin_clz(x) + 1);
#else
    uint8_t n = 1;
    while ((x & 0x80000000u) == 0) { n++; x <<= 1; }
    return n;
#endif
  }
};
#endif //HYPERLOGLOG_H

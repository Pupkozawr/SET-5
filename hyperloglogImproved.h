#ifndef HYPERLOGLOGIMPROVED_H
#define HYPERLOGLOGIMPROVED_H

#include <vector>
#include <algorithm>
#include <cstdint>
#include <string>

class HyperLogLogImproved {
public:
  HyperLogLogImproved(uint8_t B, int k)
      : B_(B), k_(k)
  {
    sketches_.reserve(k_);
    seeds_.reserve(k_);
    for (int i = 0; i < k_; ++i) {
      sketches_.emplace_back(B_);
      seeds_.push_back(0x9e3779b97f4a7c15ULL + 1234567ULL * (uint64_t)i);
    }
  }

  void add(const std::string& x) {
    for (int i = 0; i < k_; ++i) {
      uint32_t h = HashFuncGen::hash32_seeded(x, seeds_[i]);
      sketches_[i].add(h);
    }
  }

  double estimate() const {
    std::vector<double> est;
    est.reserve(k_);
    for (int i = 0; i < k_; ++i) est.push_back(sketches_[i].estimate());

    std::sort(est.begin(), est.end());
    if (k_ % 2 == 1) return est[k_/2];
    return 0.5 * (est[k_/2 - 1] + est[k_/2]);
  }

private:
  uint8_t B_;
  int k_;
  std::vector<HyperLogLog> sketches_;
  std::vector<uint64_t> seeds_;
};

#endif //HYPERLOGLOGIMPROVED_H

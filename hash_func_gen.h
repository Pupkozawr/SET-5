
#ifndef HASH_FUNC_GEN_H
#define HASH_FUNC_GEN_H

#include <string>
#include <cstdint>

class HashFuncGen {
public:
  static uint64_t fnv1a64(const std::string& s) {
    uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) {
      h ^= (uint64_t)c;
      h *= 1099511628211ULL;
    }
    return h;
  }

  static uint64_t mix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31);
    return x;
  }

  static uint32_t hash32(const std::string& s) {
    uint64_t h = mix64(fnv1a64(s));
    return (uint32_t)(h & 0xFFFFFFFFu);
  }
};

#endif //HASH_FUNC_GEN_H

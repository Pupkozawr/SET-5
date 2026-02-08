
#ifndef RANDOM_STREAM_GEN_H
#define RANDOM_STREAM_GEN_H

#include <string>
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>

class RandomStreamGen {
public:
    struct Config {
        size_t stream_len = 100000;
        size_t min_len = 1;
        size_t max_len = 30;
        uint64_t seed = 42;

        size_t universe_size = 0;
    };

    explicit RandomStreamGen(Config cfg)
        : cfg_(cfg), rng_(cfg.seed)
    {
        build_alphabet_();
        if (cfg_.universe_size > 0) {
            build_universe_();
        }
    }

    std::vector<std::string> generate_stream() {
        std::vector<std::string> s;
        s.reserve(cfg_.stream_len);

        if (cfg_.universe_size == 0) {
            for (size_t i = 0; i < cfg_.stream_len; ++i) {
                s.push_back(random_string_());
            }
        } else {
            std::uniform_int_distribution<size_t> pick(0, universe_.size() - 1);
            for (size_t i = 0; i < cfg_.stream_len; ++i) {
                s.push_back(universe_[pick(rng_)]);
            }
        }
        return s;
    }

    static std::vector<size_t> split_points(size_t n, size_t step_percent) {
        std::vector<size_t> pts;
        for (size_t p = step_percent; p <= 100; p += step_percent) {
            size_t k = (n * p) / 100;
            k = std::clamp<size_t>(k, 1, n);
            pts.push_back(k);
        }
        pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
        return pts;
    }

private:
    Config cfg_;
    std::mt19937_64 rng_;
    std::string alphabet_;
    std::vector<std::string> universe_;

    void build_alphabet_() {
        alphabet_.clear();
        for (char c = 'a'; c <= 'z'; ++c) alphabet_.push_back(c);
        for (char c = 'A'; c <= 'Z'; ++c) alphabet_.push_back(c);
        for (char c = '0'; c <= '9'; ++c) alphabet_.push_back(c);
        alphabet_.push_back('-');
    }

    std::string random_string_() {
        std::uniform_int_distribution<size_t> len_d(cfg_.min_len, cfg_.max_len);
        std::uniform_int_distribution<size_t> ch_d(0, alphabet_.size() - 1);

        size_t len = len_d(rng_);
        std::string out;
        out.resize(len);
        for (size_t i = 0; i < len; ++i) out[i] = alphabet_[ch_d(rng_)];
        return out;
    }

    void build_universe_() {
        universe_.clear();
        universe_.reserve(cfg_.universe_size);
        for (size_t i = 0; i < cfg_.universe_size; ++i) {
            universe_.push_back(random_string_());
        }
    }
};
#endif //RANDOM_STREAM_GEN_H

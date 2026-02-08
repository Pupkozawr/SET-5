#include <iostream>
#include <vector>


#include <iostream>
#include <fstream>
#include <unordered_set>
#include <numeric>
#include <cmath>

#include "random_stream_gen.h"
#include "hash_func_gen.h"
#include "hyperloglog.h"

struct StepRecord {
    size_t processed = 0;
    size_t true_distinct = 0;
    double estimate = 0.0;
};

static double mean(const std::vector<double>& x) {
    double s = std::accumulate(x.begin(), x.end(), 0.0);
    return s / double(x.size());
}

static double stdev_sample(const std::vector<double>& x) {
    if (x.size() < 2) return 0.0;
    double m = mean(x);
    double acc = 0.0;
    for (double v : x) acc += (v - m) * (v - m);
    return std::sqrt(acc / double(x.size() - 1));
}

int main() {
    const uint8_t B = 12;
    const size_t step_percent = 5;
    const size_t R = 20;

    struct StreamCfg { size_t n; size_t universe; const char* name; };
    std::vector<StreamCfg> cfgs = {
        {200000, 0,     "almost_all_unique"},
        {200000, 20000, "many_duplicates"},
        {200000, 5000,  "heavy_duplicates"}
    };

    std::ofstream per_run("per_run.csv");
    per_run << "cfg,run,step,processed,true_distinct,estimate\n";

    std::ofstream agg("aggregate.csv");
    agg << "cfg,step,processed,true_distinct_mean,estimate_mean,estimate_std\n";

    for (const auto& sc : cfgs) {
        auto split_pts = RandomStreamGen::split_points(sc.n, step_percent);
        const size_t S = split_pts.size();

        std::vector<std::vector<double>> est_by_step(S);
        std::vector<std::vector<double>> true_by_step(S);

        for (size_t run = 0; run < R; ++run) {
            RandomStreamGen::Config cfg;
            cfg.stream_len = sc.n;
            cfg.universe_size = sc.universe;
            cfg.seed = 12345 + 1000 * run + sc.universe;

            RandomStreamGen gen(cfg);
            auto stream = gen.generate_stream();

            HyperLogLog hll(B);
            std::unordered_set<std::string> seen;
            seen.reserve(sc.n);

            size_t next_idx = 0;
            size_t target = split_pts[next_idx];

            for (size_t i = 0; i < stream.size(); ++i) {
                const auto& x = stream[i];
                seen.insert(x);
                uint32_t h = HashFuncGen::hash32(x);
                hll.add(h);
                size_t processed = i + 1;
                if (processed == target) {
                    size_t F0 = seen.size();
                    double Nt = hll.estimate();

                    per_run << sc.name << "," << run << "," << next_idx
                            << "," << processed << "," << F0 << "," << Nt << "\n";

                    true_by_step[next_idx].push_back(double(F0));
                    est_by_step[next_idx].push_back(double(Nt));

                    next_idx++;
                    if (next_idx >= S) break;
                    target = split_pts[next_idx];
                }
            }
        }

        for (size_t step = 0; step < split_pts.size(); ++step) {
            double t_mean = mean(true_by_step[step]);
            double e_mean = mean(est_by_step[step]);
            double e_std  = stdev_sample(est_by_step[step]);

            agg << sc.name << "," << step << "," << split_pts[step]
                << "," << t_mean << "," << e_mean << "," << e_std << "\n";
        }
    }

    std::cout << "Done. Wrote per_run.csv and aggregate.csv\n";
    return 0;
}
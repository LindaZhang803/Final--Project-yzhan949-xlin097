#include "../head/bsp.h"

#include <algorithm>
#include <stdexcept>
#include <thread>
#include <vector>

void BspSerial(const CSRGraph& g, BspAlgorithm& algo) {
    while (algo.HasWork()) {
        for (int v = 0; v < g.num_vertices; v++) {
            algo.Process(0, v, g);
        }
        algo.PostRound();
    }
}

static void BspWorker(const CSRGraph& g, BspAlgorithm& algo, int tid, int start, int end) {
    for (int v = start; v < end; v++) {
        algo.Process(tid, v, g);
    }
}

void BspParallel(const CSRGraph& g, BspAlgorithm& algo, int num_threads) {
    if (num_threads <= 0) {
        throw std::invalid_argument("num_threads must be > 0");
    }

    if (g.num_vertices == 0) {
        return;
    }

    int actual_threads = std::min(num_threads, g.num_vertices);

    while (algo.HasWork()) {
        int chunk = (g.num_vertices + actual_threads - 1) / actual_threads;

        std::vector<std::thread> threads;
        threads.reserve(actual_threads);

        for (int tid = 0; tid < actual_threads; tid++) {
            int start = tid * chunk;
            int end = std::min(start + chunk, g.num_vertices);
            if (start >= g.num_vertices) {
                break;
            }

            threads.push_back(std::thread(BspWorker, std::cref(g), std::ref(algo), tid, start, end));
        }

        for (auto& th : threads) {
            th.join();
        }

        algo.PostRound();
    }
}

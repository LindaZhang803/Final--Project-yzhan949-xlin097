#include "../head/algorithms.h"

#include <algorithm>

const long long Sssp::INF = std::numeric_limits<long long>::max() / 4;

// ========================= BFS =========================

Bfs::Bfs(int num_vertices, int source, int num_threads)
    : source_(source),
      num_threads_(num_threads),
      has_work_(true),
      dist_(num_vertices, -1),
      dist_prev_(num_vertices, -1),
      tl_updated_(num_threads, 0) {
    if (source_ >= 0 && source_ < num_vertices) {
        dist_[source_] = 0;
        dist_prev_[source_] = 0;
    }
}

bool Bfs::HasWork() const {
    return has_work_;
}

void Bfs::Process(int tid, int v, const CSRGraph& g) {
    int best = dist_prev_[v];
    bool updated = false;

    for (int i = g.in_offsets[v]; i < g.in_offsets[v + 1]; i++) {
        int u = g.in_edges[i];
        if (dist_prev_[u] == -1) {
            continue;
        }

        int candidate = dist_prev_[u] + 1;
        if (best == -1 || candidate < best) {
            best = candidate;
            updated = true;
        }
    }

    if (updated) {
        dist_[v] = best;
        tl_updated_[tid] = 1;
    }
}

void Bfs::PostRound() {
    bool any_updated = false;

    for (int t = 0; t < num_threads_; t++) {
        if (tl_updated_[t]) {
            any_updated = true;
        }
        tl_updated_[t] = 0;
    }

    dist_prev_ = dist_;
    has_work_ = any_updated;
}

const std::vector<int>& Bfs::GetDistances() const {
    return dist_;
}

// ========================= SSSP =========================

Sssp::Sssp(int num_vertices, int source, int num_threads)
    : source_(source),
      num_threads_(num_threads),
      has_work_(true),
      dist_(num_vertices, INF),
      dist_prev_(num_vertices, INF),
      tl_updated_(num_threads, 0) {
    if (source_ >= 0 && source_ < num_vertices) {
        dist_[source_] = 0;
        dist_prev_[source_] = 0;
    }
}

bool Sssp::HasWork() const {
    return has_work_;
}

void Sssp::Process(int tid, int v, const CSRGraph& g) {
    long long best = dist_prev_[v];
    bool updated = false;

    for (int i = g.in_offsets[v]; i < g.in_offsets[v + 1]; i++) {
        int u = g.in_edges[i];
        int w = g.in_weights[i];

        if (dist_prev_[u] == INF) {
            continue;
        }

        long long candidate = dist_prev_[u] + w;
        if (candidate < best) {
            best = candidate;
            updated = true;
        }
    }

    if (updated) {
        dist_[v] = best;
        tl_updated_[tid] = 1;
    }
}

void Sssp::PostRound() {
    bool any_updated = false;

    for (int t = 0; t < num_threads_; t++) {
        if (tl_updated_[t]) {
            any_updated = true;
        }
        tl_updated_[t] = 0;
    }

    dist_prev_ = dist_;
    has_work_ = any_updated;
}

const std::vector<long long>& Sssp::GetDistances() const {
    return dist_;
}

// ========================= CC =========================

Cc::Cc(int num_vertices, int num_threads)
    : num_threads_(num_threads),
      has_work_(true),
      comp_(num_vertices, 0),
      comp_prev_(num_vertices, 0),
      tl_updated_(num_threads, 0) {
    for (int i = 0; i < num_vertices; i++) {
        comp_[i] = i;
        comp_prev_[i] = i;
    }
}

bool Cc::HasWork() const {
    return has_work_;
}

void Cc::Process(int tid, int v, const CSRGraph& g) {
    int best = comp_prev_[v];
    bool updated = false;

    for (int i = g.in_offsets[v]; i < g.in_offsets[v + 1]; i++) {
        int u = g.in_edges[i];
        if (comp_prev_[u] < best) {
            best = comp_prev_[u];
            updated = true;
        }
    }

    for (int i = g.offsets[v]; i < g.offsets[v + 1]; i++) {
        int u = g.edges[i];
        if (comp_prev_[u] < best) {
            best = comp_prev_[u];
            updated = true;
        }
    }

    if (updated) {
        comp_[v] = best;
        tl_updated_[tid] = 1;
    }
}

void Cc::PostRound() {
    bool any_updated = false;

    for (int t = 0; t < num_threads_; t++) {
        if (tl_updated_[t]) {
            any_updated = true;
        }
        tl_updated_[t] = 0;
    }

    comp_prev_ = comp_;
    has_work_ = any_updated;
}

const std::vector<int>& Cc::GetComponents() const {
    return comp_;
}
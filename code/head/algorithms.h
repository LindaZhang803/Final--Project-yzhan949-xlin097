#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "bsp.h"

#include <vector>
#include <limits>

class Bfs : public BspAlgorithm {
public:
    Bfs(int num_vertices, int source, int num_threads);

    bool HasWork() const override;
    void Process(int tid, int v, const CSRGraph& g) override;
    void PostRound() override;

    const std::vector<int>& GetDistances() const;

private:
    int source_;
    int num_threads_;
    bool has_work_;

    std::vector<int> dist_;
    std::vector<int> dist_prev_;
    std::vector<int> tl_updated_;
};

class Sssp : public BspAlgorithm {
public:
    Sssp(int num_vertices, int source, int num_threads);

    bool HasWork() const override;
    void Process(int tid, int v, const CSRGraph& g) override;
    void PostRound() override;

    const std::vector<long long>& GetDistances() const;

private:
    static const long long INF;

    int source_;
    int num_threads_;
    bool has_work_;

    std::vector<long long> dist_;
    std::vector<long long> dist_prev_;
    std::vector<int> tl_updated_;
};

class Cc : public BspAlgorithm {
public:
    Cc(int num_vertices, int num_threads);

    bool HasWork() const override;
    void Process(int tid, int v, const CSRGraph& g) override;
    void PostRound() override;

    const std::vector<int>& GetComponents() const;

private:
    int num_threads_;
    bool has_work_;

    std::vector<int> comp_;
    std::vector<int> comp_prev_;
    std::vector<int> tl_updated_;
};

#endif
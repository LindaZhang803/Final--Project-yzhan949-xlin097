#ifndef QUERY_H
#define QUERY_H

#include "graph.h"

#include <functional>
#include <string>
#include <vector>

using QueryCallback = std::function<std::string(const CSRGraph&, int src, int K)>;

struct QueryTask {
    int src = 0;
    int K = 0;
    int query_type = 0;
    int expected_result = 0;
    QueryCallback cb;
    std::string result;
};

std::string CountKHopReachable(const CSRGraph& g, int src, int K);
std::string MaxKHopReachable(const CSRGraph& g, int src, int K);

std::vector<QueryTask> LoadQueryTasks(const std::string& query_file);

void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks);

#endif

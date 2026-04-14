#pragma once

#include <functional>
#include <string>

struct CSRGraph;

using QueryCallback = std::function<std::string(const CSRGraph&, int, int)>;

struct QueryTask {
    int src;
    int K;
    QueryCallback cb;
    std::string result;
};
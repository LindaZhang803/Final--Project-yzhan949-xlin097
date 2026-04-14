#pragma once

#include <vector>
#include "graph.h"
#include "query.h"

void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks);
void RunTasksParallel(const CSRGraph& g, std::vector<QueryTask>& tasks, int num_threads);
bool CheckResultsEqual(const std::vector<QueryTask>& a, const std::vector<QueryTask>& b);
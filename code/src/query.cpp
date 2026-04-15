#include "query.h"

#include <fstream>
#include <sstream>
#include <stdexcept>



std::string CountKHopReachable(const CSRGraph& g, int src, int K) {
    if (K <= 0 || src < 0 || src >= g.num_vertices) {
        return "0";
    }

    std::vector<int> visited(g.num_vertices, 0);
    std::vector<std::pair<int, int>> frontier;
    frontier.push_back(std::make_pair(src, 0));
    visited[src] = 1;

    int count = 0;

    for (size_t head = 0; head < frontier.size(); ++head) {
        int u = frontier[head].first;
        int depth = frontier[head].second;
        if (depth == K) {
            continue;
        }

        for (int i = g.offsets[u]; i < g.offsets[u + 1]; ++i) {
            int v = g.edges[i];
            if (!visited[v]) {
                visited[v] = 1;
                count++;
                frontier.push_back(std::make_pair(v, depth + 1));
            }
        }
    }

    return std::to_string(count);
}

std::string MaxKHopReachable(const CSRGraph& g, int src, int K) {
    if (K <= 0 || src < 0 || src >= g.num_vertices) {
        return "-1";
    }

    std::vector<int> visited(g.num_vertices, 0);
    std::vector<std::pair<int, int>> frontier;
    frontier.push_back(std::make_pair(src, 0));
    visited[src] = 1;

    int max_id = -1;

    for (size_t head = 0; head < frontier.size(); ++head) {
        int u = frontier[head].first;
        int depth = frontier[head].second;
        if (depth == K) {
            continue;
        }

        for (int i = g.offsets[u]; i < g.offsets[u + 1]; ++i) {
            int v = g.edges[i];
            if (!visited[v]) {
                visited[v] = 1;
                if (v > max_id) {
                    max_id = v;
                }
                frontier.push_back(std::make_pair(v, depth + 1));
            }
        }
    }

    return std::to_string(max_id);
}

std::vector<QueryTask> LoadQueryTasks(const std::string& query_file) {
    std::ifstream file(query_file);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open query file: " + query_file);
    }

    std::vector<QueryTask> tasks;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        QueryTask task;
        if (!(ss >> task.src >> task.K >> task.query_type >> task.expected_result)) {
            continue;
        }

        if (task.query_type == 1) {
            task.cb = CountKHopReachable;
        } else if (task.query_type == 2) {
            task.cb = MaxKHopReachable;
        } else {
            throw std::runtime_error("unsupported query type: " + std::to_string(task.query_type));
        }

        tasks.push_back(task);
    }

    return tasks;
}

void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks) {
    for (auto& task : tasks) {
        if (task.cb) {
            task.result = task.cb(g, task.src, task.K);
        } else {
            task.result.clear();
        }
    }
}

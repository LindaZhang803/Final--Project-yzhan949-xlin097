#include "../head/graph.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

struct WeightedEdge {
    int src;
    int dst;
    int weight;
};

CSRGraph LoadGraph(const std::string& filename) {
    CSRGraph g;

    std::ifstream file(filename);
    std::string line;

    std::vector<std::pair<int, int>> edge_list;
    int max_id = -1;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        int src, dst;
        ss >> src >> dst;

        edge_list.push_back({src, dst});

        if (src > max_id) {
            max_id = src;
        }
        if (dst > max_id) {
            max_id = dst;
        }
    }

    g.num_vertices = max_id + 1;

    std::vector<std::vector<int>> adj(g.num_vertices);

    for (auto &e : edge_list) {
        int src = e.first;
        int dst = e.second;
        adj[src].push_back(dst);
    }

    g.offsets.resize(g.num_vertices + 1);
    g.offsets[0] = 0;

    for (int i = 0; i < g.num_vertices; i++) {
        g.offsets[i + 1] = g.offsets[i] + adj[i].size();
    }

    g.edges.resize(g.offsets[g.num_vertices]);

    int idx = 0;
    for (int i = 0; i < g.num_vertices; i++) {
        for (int v : adj[i]) {
            g.edges[idx] = v;
            idx++;
        }
    }

    return g;
}

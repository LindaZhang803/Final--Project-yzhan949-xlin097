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
    std::ifstream file(filename);
    std::string line;

    std::vector<WeightedEdge> edge_list;
    int max_id = -1;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        int src = -1;
        int dst = -1;
        int weight = 1;
        if (!(ss >> src >> dst)) {
            continue;
        }
        if (!(ss >> weight)) {
            weight = 1;
        }

        edge_list.push_back({src, dst, weight});

        if (src > max_id) {
            max_id = src;
        }
        if (dst > max_id) {
            max_id = dst;
        }
    }

    CSRGraph g;
    g.num_vertices = max_id + 1;

    if (g.num_vertices <= 0) {
        g.num_vertices = 0;
        g.offsets.assign(1, 0);
        g.in_offsets.assign(1, 0);
        return g;
    }

    std::vector<int> out_degree(g.num_vertices, 0);
    std::vector<int> in_degree(g.num_vertices, 0);

    for (const auto& e : edge_list) {
        out_degree[e.src]++;
        in_degree[e.dst]++;
    }

    g.offsets.resize(g.num_vertices + 1);
    g.in_offsets.resize(g.num_vertices + 1);
    g.offsets[0] = 0;
    g.in_offsets[0] = 0;

    for (int i = 0; i < g.num_vertices; i++) {
        g.offsets[i + 1] = g.offsets[i] + out_degree[i];
        g.in_offsets[i + 1] = g.in_offsets[i] + in_degree[i];
    }

    g.edges.assign(g.offsets[g.num_vertices], 0);
    g.weights.assign(g.offsets[g.num_vertices], 0);
    g.in_edges.assign(g.in_offsets[g.num_vertices], 0);
    g.in_weights.assign(g.in_offsets[g.num_vertices], 0);

    std::vector<int> out_cursor = g.offsets;
    std::vector<int> in_cursor = g.in_offsets;

    for (const auto& e : edge_list) {
        int out_idx = out_cursor[e.src]++;
        g.edges[out_idx] = e.dst;
        g.weights[out_idx] = e.weight;

        int in_idx = in_cursor[e.dst]++;
        g.in_edges[in_idx] = e.src;
        g.in_weights[in_idx] = e.weight;
    }

    return g;
}

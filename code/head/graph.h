#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

struct CSRGraph {
    int num_vertices = 0;
    std::vector<int> offsets;
    std::vector<int> edges;
    std::vector<int> weights;
    std::vector<int> in_offsets;
    std::vector<int> in_edges;
    std::vector<int> in_weights;
};

CSRGraph LoadGraph(const std::string& filename);

#endif

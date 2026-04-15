#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

struct CSRGraph {
    int num_vertices = 0;
    std::vector<int> offsets;
    std::vector<int> edges;
};

CSRGraph LoadGraph(const std::string& filename);

#endif

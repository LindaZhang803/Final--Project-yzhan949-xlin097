#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

struct CSRGraph {
    int num_vertices;
    vector<int> offsets;
    vector<int> edges;
};

CSRGraph LoadGraph(const char *filename) {
    CSRGraph g;

    ifstream file(filename);
    string line;

    vector<pair<int, int>> edge_list;
    int max_id = -1;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        stringstream ss(line);
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

    vector<vector<int>> adj(g.num_vertices);

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

int main() {
    CSRGraph g = LoadGraph("./code/soc-Slashdot0902.txt");

    cout << "num_vertices = " << g.num_vertices << endl;
    cout << "num_edges = " << g.edges.size() << endl;

    // print first few offsets
    cout << "first 10 offsets: ";
    for (int i = 0; i < 10 && i < g.offsets.size(); i++) {
        cout << g.offsets[i] << " ";
    }
    cout << endl;

    // print first few edges
    cout << "first 10 edges: ";
    for (int i = 0; i < 10 && i < g.edges.size(); i++) {
        cout << g.edges[i] << " ";
    }
    cout << endl;

    return 0;
}

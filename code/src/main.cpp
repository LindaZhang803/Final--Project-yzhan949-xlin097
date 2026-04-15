#include "graph.h"
#include "query.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        // defaults
        std::string graph_file = "soc-Slashdot0902.txt";
        std::string query_file = "queries20.txt";

        if (argc >= 2) {
            graph_file = argv[1];
        }
        if (argc >= 3) {
            query_file = argv[2];
        }

        std::cout << "Loading graph: " << graph_file << '\n';
        CSRGraph graph = LoadGraph(graph_file);
        std::cout << "Vertices: " << graph.num_vertices << "\nEdges: " << graph.edges.size() << '\n';

        std::cout << "Loading queries: " << query_file << '\n';
        std::vector<QueryTask> tasks = LoadQueryTasks(query_file);
        std::cout << "Queries: " << tasks.size() << '\n';

        RunTasksSequential(graph, tasks);

        int correct = 0;
        for (const auto& task : tasks) {
            std::cout << task.src << ' ' << task.K << ' ' << task.query_type << " -> " << task.result << '\n';
            if (task.result == std::to_string(task.expected_result)) {
                ++correct;
            }
        }
        std::cout << "Correct queries: " << correct << "/" << tasks.size() << '\n';

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}

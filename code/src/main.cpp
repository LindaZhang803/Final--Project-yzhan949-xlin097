#include "../head/graph.h"
#include "../head/query.h"
#include "../head/parallel.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <chrono>

int main(int argc, char* argv[]) {
    try {
        // defaults
        std::string graph_file = "soc-Slashdot0902.txt";
        std::string query_file = "queries10000.txt";

        if (argc >= 2) {
            graph_file = argv[1];
        }
        if (argc >= 3) {
            query_file = argv[2];
        }

        std::cout << "Loading graph: " << graph_file << '\n';
        std::cout << std::endl;
        CSRGraph graph = LoadGraph(graph_file);
        std::cout << "Vertices: " << graph.num_vertices << "\nEdges: " << graph.edges.size() << '\n';
        std::cout << std::endl;

        std::cout << "Loading queries: " << query_file << '\n';
        std::cout << std::endl;
        std::vector<QueryTask> tasks = LoadQueryTasks(query_file);
        std::cout << "Queries: " << tasks.size() << '\n';
        std::cout << std::endl;

        RunTasksSequential(graph, tasks);

        RunTasksParallel(graph, tasks, 4);
   
        if(!CheckResultsEqual(tasks, tasks)) {
            std::cerr << "Error: Parallel results do not match sequential results!\n";
            std::cout << std::endl;
        }else{
            std::cout << "Parallel results match sequential results.\n";
            std::cout << std::endl;
        }

        int correct = 0;
        for (const auto& task : tasks) {
            std::cout << task.src << ' ' << task.K << ' ' << task.query_type << " -> " << task.result << '\n';
            if (task.result == std::to_string(task.expected_result)) {
                ++correct;
            }
        }
        std::cout << "Correct queries: " << correct << "/" << tasks.size() << '\n';
        std::cout << std::endl;

        auto t1 = std::chrono::high_resolution_clock::now();
        RunTasksParallel(graph, tasks, 4);
        auto t2 = std::chrono::high_resolution_clock::now();

        // Calculate and print the time taken for parallel execution
        auto t3 = std::chrono::high_resolution_clock::now();
        RunTasksParallel(graph, tasks, 4);
        auto t4 = std::chrono::high_resolution_clock::now();

        auto seq_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count(); 
        auto par_time = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();

        std::cout << "Time taken for parallel execution: " << par_time << " ms\n";
        std::cout << std::endl;
        std::cout << "Time taken for sequential execution: " << seq_time << " ms\n";
        std::cout << std::endl;

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        std::cout << std::endl;
        return 1;
    }
}

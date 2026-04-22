#include "../head/graph.h"
#include "../head/bsp.h"
#include "../head/algorithms.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static void WriteBfsFile(const std::string& filename, const std::vector<int>& dist) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("failed to open output file: " + filename);
    }

    for (int i = 0; i < (int)dist.size(); i++) {
        out << i << ' ' << dist[i] << '\n';
    }
}

static void WriteSsspFile(const std::string& filename, const std::vector<long long>& dist) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("failed to open output file: " + filename);
    }

    for (int i = 0; i < (int)dist.size(); i++) {
        if (dist[i] == std::numeric_limits<long long>::max() / 4) {
            out << i << " -1\n";
        } else {
            out << i << ' ' << dist[i] << '\n';
        }
    }
}

static void WriteCcFile(const std::string& filename, const std::vector<int>& comp) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("failed to open output file: " + filename);
    }

    for (int i = 0; i < (int)comp.size(); i++) {
        out << i << ' ' << comp[i] << '\n';
    }
}

static bool EqualIntVector(const std::vector<int>& a, const std::vector<int>& b) {
    return a == b;
}

static bool EqualLongLongVector(const std::vector<long long>& a, const std::vector<long long>& b) {
    return a == b;
}

int main(int argc, char* argv[]) {
    try {
        std::string graph_file = "file/soc-LiveJournal1-weighted.txt";
        if (argc >= 2) {
            graph_file = argv[1];
        }

        std::cout << "Loading graph: " << graph_file << '\n';
        CSRGraph graph = LoadGraph(graph_file);
        std::cout << "Vertices: " << graph.num_vertices << '\n';
        std::cout << "Edges: " << graph.edges.size() << "\n\n";

        int source = 0;
        int num_threads = 4;

        // ================= BFS =================
        auto bfs_s_t1 = std::chrono::high_resolution_clock::now();
        Bfs bfs_serial(graph.num_vertices, source, 1);
        BspSerial(graph, bfs_serial);
        auto bfs_s_t2 = std::chrono::high_resolution_clock::now();

        auto bfs_p_t1 = std::chrono::high_resolution_clock::now();
        Bfs bfs_parallel(graph.num_vertices, source, num_threads);
        BspParallel(graph, bfs_parallel, num_threads);
        auto bfs_p_t2 = std::chrono::high_resolution_clock::now();

        bool bfs_equal = EqualIntVector(bfs_serial.GetDistances(), bfs_parallel.GetDistances());

        // ================= SSSP =================
        auto sssp_s_t1 = std::chrono::high_resolution_clock::now();
        Sssp sssp_serial(graph.num_vertices, source, 1);
        BspSerial(graph, sssp_serial);
        auto sssp_s_t2 = std::chrono::high_resolution_clock::now();

        auto sssp_p_t1 = std::chrono::high_resolution_clock::now();
        Sssp sssp_parallel(graph.num_vertices, source, num_threads);
        BspParallel(graph, sssp_parallel, num_threads);
        auto sssp_p_t2 = std::chrono::high_resolution_clock::now();

        bool sssp_equal = EqualLongLongVector(sssp_serial.GetDistances(), sssp_parallel.GetDistances());

        // ================= CC =================
        auto cc_s_t1 = std::chrono::high_resolution_clock::now();
        Cc cc_serial(graph.num_vertices, 1);
        BspSerial(graph, cc_serial);
        auto cc_s_t2 = std::chrono::high_resolution_clock::now();

        auto cc_p_t1 = std::chrono::high_resolution_clock::now();
        Cc cc_parallel(graph.num_vertices, num_threads);
        BspParallel(graph, cc_parallel, num_threads);
        auto cc_p_t2 = std::chrono::high_resolution_clock::now();

        bool cc_equal = EqualIntVector(cc_serial.GetComponents(), cc_parallel.GetComponents());

        auto bfs_serial_ms = std::chrono::duration_cast<std::chrono::milliseconds>(bfs_s_t2 - bfs_s_t1).count();
        auto bfs_parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(bfs_p_t2 - bfs_p_t1).count();

        auto sssp_serial_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sssp_s_t2 - sssp_s_t1).count();
        auto sssp_parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(sssp_p_t2 - sssp_p_t1).count();

        auto cc_serial_ms = std::chrono::duration_cast<std::chrono::milliseconds>(cc_s_t2 - cc_s_t1).count();
        auto cc_parallel_ms = std::chrono::duration_cast<std::chrono::milliseconds>(cc_p_t2 - cc_p_t1).count();

        std::cout << "BFS   serial=" << bfs_serial_ms << " ms, parallel=" << bfs_parallel_ms
                  << " ms, equal=" << (bfs_equal ? "YES" : "NO") << '\n';
        std::cout << "SSSP  serial=" << sssp_serial_ms << " ms, parallel=" << sssp_parallel_ms
                  << " ms, equal=" << (sssp_equal ? "YES" : "NO") << '\n';
        std::cout << "CC    serial=" << cc_serial_ms << " ms, parallel=" << cc_parallel_ms
                  << " ms, equal=" << (cc_equal ? "YES" : "NO") << '\n';

        if (!bfs_equal || !sssp_equal || !cc_equal) {
            std::cerr << "\nError: serial and parallel results do not match.\n";
            return 1;
        }

        WriteBfsFile("BFS.txt", bfs_serial.GetDistances());
        WriteSsspFile("SSSP.txt", sssp_serial.GetDistances());
        WriteCcFile("CC.txt", cc_serial.GetComponents());

        std::cout << "\nOutput files written: BFS.txt, SSSP.txt, CC.txt\n";

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}

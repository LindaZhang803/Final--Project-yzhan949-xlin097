# Concurrent Graph Analytics Engine

# Phase 1: Concurrent Local Queries

## Introduction

In this phase, you will load a directed graph from an edge list file into memory using the Compressed Sparse Row (CSR) format, and implement K-hop neighborhood queries. Each query explores vertices reachable within K hops from a given source vertex (following outgoing edges) and computes a result using a user-defined callback.

You will then evaluate a sequence of such queries, first sequentially and then concurrently using multiple threads, and compare their execution time.

## Requirements

1. **Graph Loading**: Read an input graph in edge list format and store it in memory using the CSR representation.

2. **Query Callback**: Provide a callback interface that allows developers to implement custom logic for processing the K-hop query results. You should support at least the following queries:
   - Count the number of reachable nodes within K hops (excluding the source vertex itself).
   - Find the maximum node ID among the reachable nodes within K hops (excluding the source vertex itself).

3. **Sequential Evaluation**: Execute a given sequence of K-hop queries one by one and record the total execution time.

4. **Concurrent Evaluation**: Execute the same sequence of K-hop queries concurrently using multiple threads and record the total execution time.

5. **Performance Comparison**: Measure and compare the execution time between sequential and concurrent evaluation. Report your findings with analysis in the development journal.

## Hints

### Edge List Format

The input graph is stored as a text file where each line represents a directed edge:

```
src dst
```

Lines starting with `#` are comments and should be skipped. Node IDs are integers. Example:

```
# example graph
# FromNodeId ToNodeId
0 1
0 2
1 3
2 3
```

### Query File Format

A query file is a text file where each line specifies a query:

```
src K queryType expectedResult
```

- `queryType`: 1 = count reachable nodes, 2 = max node ID (returns -1 if no reachable nodes)
- `expectedResult`: the expected output for correctness verification

Example:

```
# src K queryType(1=count,2=max) expectedResult
15795 1 1 20
76820 3 2 53604
37194 3 2 -1
```

We provide two query files: `queries20.txt` (20 queries, for debugging) and `queries10000.txt` (10,000 queries, for performance evaluation).

### Data Structures & Interfaces

The following definitions are provided as a starting point. Feel free to adapt or redesign them to better suit your implementation.

```cpp
struct CSRGraph {
    int num_vertices;
    std::vector<int> offsets;  // size = num_vertices + 1
    std::vector<int> edges;    // concatenated adjacency lists
    // offsets[i]..offsets[i+1] gives the range of neighbors for vertex i in edges[]
};

using QueryCallback = std::function<std::string(const CSRGraph&, int src, int K)>;

struct QueryTask {
    int src;
    int K;
    QueryCallback cb;
    std::string result;
};

void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks);

// execute tasks concurrently with num_threads threads
void RunTasksParallel(const CSRGraph& g, std::vector<QueryTask>& tasks, int num_threads);
```

### Pseudocode

The following pseudocode illustrates the overall workflow. It is provided as a reference, and you may organize your code differently.

```
// 1. Load graph
graph = LoadGraph("edges.txt")

// 2. Define callbacks
count_cb = (graph, src, K) -> return count of K-hop reachable nodes
max_cb   = (graph, src, K) -> return max node ID among K-hop reachable nodes

// 3. Load query file and build tasks
for each line (src, K, queryType, expectedResult) in "queries.txt":
    cb = (queryType == 1) ? count_cb : max_cb
    tasks.append(QueryTask(src, K, cb))

// 4. Run sequentially, record time
seq_tasks = copy(tasks)
t1 = now()
RunTasksSequential(graph, seq_tasks)
sequential_time = now() - t1

// 5. Run concurrently, record time
par_tasks = copy(tasks) // copy avoid overwriting results
t2 = now()
RunTasksParallel(graph, par_tasks, num_threads=4)
parallel_time = now() - t2

// 6. Compare and report
print(sequential_time, parallel_time)
```

## Testing & Verification

To verify the correctness of your implementation, we recommend the following approach:

1. **Construct your own test cases**: Start with small graphs where you can manually compute the expected K-hop results. For example:
   - A graph with cycles to ensure visited nodes are not revisited
   - A star graph to test fan-out behavior
   - Edge cases: isolated vertices, K=0, source vertex not in graph

2. **Compare sequential and concurrent results**: Run the same set of queries using both `RunTasksSequential` and `RunTasksParallel`, and verify that they produce identical results.

3. **Verify on the provided dataset**: Once small cases pass, run your implementations on the provided dataset ([download link](https://drive.google.com/drive/folders/1Cr4QkLBpWa3Gp0u-9YWNE9voH9Dz4INg)). The dataset includes `soc-Slashdot0902.txt` (the edge list file) and query files. Use `queries20.txt` to debug correctness by comparing your output against the expected results. Then use `queries10000.txt` for performance evaluation. You should expect a noticeable speedup from the concurrent version. Report the correctness and performance results on this dataset in your development journal.

# Phase 2a: Parallel Global Queries

## Introduction

In this phase, you will implement three classical graph algorithms (BFS, SSSP, and CC) using the Bulk Synchronous Parallel (BSP) execution model.

BSP divides the computation into rounds. In each round:
1. **Process** every vertex (in parallel when multiple threads are used).
2. **Synchronize**: wait for all threads to finish the round.
3. **Finalize** (single-threaded): combine per-thread results and set up state for the next round.

Repeat until a full round produces no updates.

You will implement a small BSP framework (a serial runner and a parallel runner built on `std::thread`) and plug in three algorithm callbacks. The framework should be designed so that each algorithm is written once and can be executed by either runner unchanged.

## Requirements

1. **Graph Loading**: Extend the CSR representation from Phase 1 to carry:
   - Edge weights
   - A reverse CSR (in-edges) alongside the forward CSR

2. **BSP Framework**: Design a small framework that separates *what an algorithm does per vertex* from *how vertices are scheduled in a round*. Your framework should include:
   - An interface (e.g., an abstract base class) that an algorithm plugs into by providing a per-vertex callback, end-of-round bookkeeping, and a way to tell the runner when to stop.
   - A serial runner.
   - A parallel runner built on `std::thread`.

   Design the interface so that the **same algorithm implementation** runs unchanged under either runner.

3. **Algorithms**: Implement the following three algorithms as callbacks to your framework. Write each algorithm **once**; the same code should run under both runners.
   - **BFS**: shortest hop-distance from a source vertex.
   - **SSSP**: shortest weighted distance from a source vertex (Bellman-Ford style relaxation).
   - **CC**: weakly connected components via label propagation. Treat edges as undirected when propagating labels (each vertex adopts the minimum label among all its neighbors, in either direction).

4. **Correctness**: For each algorithm, the serial and parallel runs must produce identical per-vertex results.

5. **Performance Comparison**: Measure and compare execution times for the serial and parallel versions, and report speedup in the development journal.

## Hints

### Weighted Edge List Format

The input graph is a tab-separated text file with three columns:

```
src	dst	weight
```

Lines starting with `#` are comments. Example:

```
# FromNodeId	ToNodeId	Weight
0	1	194
0	2	221
1	3	76
2	3	175
```

### Answer File Format

Each algorithm writes one line per vertex, sorted by vertex ID. The two columns are separated by a space.

- **BFS**: `vertex_id hop_distance`. Hop distance from the source, or `-1` if unreachable.
- **SSSP**: `vertex_id weighted_distance`. Shortest weighted distance from the source, or `-1` if unreachable.
- **CC**: `vertex_id component_id`. The component ID is the smallest vertex ID in the weakly connected component containing `vertex_id`.

### Data Structures & Interfaces

The following definitions are provided as a starting point. Feel free to adapt or redesign them to better suit your implementation.

```cpp
struct CsrGraph {
    uint32_t num_vertices;
    std::vector<uint32_t> offsets;      // out-edges, size = num_vertices + 1
    std::vector<uint32_t> edges;        // out-neighbor IDs
    std::vector<int> weights;           // edge weights (parallel to edges)
    std::vector<uint32_t> in_offsets;   // in-edges (reverse CSR)
    std::vector<uint32_t> in_edges;     // in-neighbor IDs
    std::vector<int> in_weights;        // reverse edge weights (parallel to in_edges)
};

class BspAlgorithm {
public:
    virtual ~BspAlgorithm() = default;

    // Returns true if another round should be run.
    virtual bool HasWork() const = 0;

    // Process one vertex. tid ∈ [0, nthreads). The serial runner calls with
    // tid=0; the parallel runner calls with each thread's ID.
    virtual void Process(int tid, uint32_t v, const CsrGraph& g) = 0;

    // End of round. Merge per-thread buffers, prepare state for next round.
    virtual void PostRound() = 0;
};

void BspSerial(const CsrGraph& g, BspAlgorithm& algo);
void BspParallel(const CsrGraph& g, BspAlgorithm& algo, int nthreads);
```

Each of BFS, SSSP, and CC is implemented as a subclass of `BspAlgorithm` (e.g., `class Bfs : public BspAlgorithm`), then passed into a runner:

```cpp
Bfs bfs(g.num_vertices, /*source=*/0, /*nthreads=*/1);
BspSerial(g, bfs);

Bfs bfs_p(g.num_vertices, /*source=*/0, /*nthreads=*/4);
BspParallel(g, bfs_p, /*nthreads=*/4);
```

A minimal `BspSerial` is just a loop over the three virtual methods:

```cpp
void BspSerial(const CsrGraph& g, BspAlgorithm& algo) {
  while (algo.HasWork()) {
    for (uint32_t v = 0; v < g.num_vertices; v++) {
      algo.Process(/*tid=*/0, v, g);
    }
    algo.PostRound();
  }
}
```

`BspParallel` has the same outer structure. The only change is inside the round: instead of one thread iterating every vertex, partition `[0, num_vertices)` into `nthreads` contiguous ranges, spawn an `std::thread` per range (each calling `Process()` with its own `tid`), and wait for all threads to finish before calling `PostRound()`.

### Algorithm Design

Since the same algorithm must run under both runners, design `Process(tid, v, g)` so it only **reads** neighbor state and only **writes** `v`'s own state. The parallel runner assigns each vertex to exactly one thread (static partitioning of `[0, num_vertices)`), so `v`'s state is written by one thread per round.

Every round, every vertex `v` scans its in-edges (via the reverse CSR) and takes the best improvement, if any, from any in-neighbor. The algorithm is **done** when a full round produces no updates.

```
Process(tid, v, g):
    best = current state of v
    updated = false
    for u in in_neighbors(v):
        candidate = value derived from state_prev[u]
                    //   BFS:  state_prev[u] + 1
                    //   SSSP: state_prev[u] + weight(u, v)
                    //   CC:   state_prev[u]
        if candidate improves best:
            best = candidate
            updated = true
    if updated:
        state of v = best
        tl_updated[tid] = 1
```

**Signaling convergence.** Each thread sets `tl_updated[tid] = 1` when it updates at least one vertex during the round. `PostRound()` reduces the per-thread flags into a single `any_updated` boolean; `HasWork()` returns that value, so the loop exits as soon as a round settles everything.

**Double buffering.** A vertex's value can be improved more than once (SSSP finds shorter paths, CC finds smaller labels). If thread A reads `state[u]` while thread B is simultaneously writing `state[u]`, the read is unsafe. To prevent this, keep two arrays: `state_` (live, written by `Process()`) and `state_prev_` (read by `Process()`, a snapshot of the values at the end of the previous round). `PostRound()` copies `state_` into `state_prev_` so the next round reads a stable view.

> **Note on CC**: CC computes weakly connected components, so labels should propagate across every edge regardless of direction. For CC, `Process()` must scan both in-neighbors and out-neighbors.

### Overall Workflow

The following pseudocode illustrates the overall workflow. It is provided as a reference, and you may organize your code differently.

```
// 1. Load weighted graph with reverse edges
graph = LoadGraph("edge.txt")

// 2. Run BFS with both runners. The same Bfs class is used by both.
bfs_s = Bfs(graph.num_vertices, source=0, nthreads=1)
BspSerial(graph, bfs_s)

bfs_p = Bfs(graph.num_vertices, source=0, nthreads=4)
BspParallel(graph, bfs_p, nthreads=4)

// 3. Verify that serial and parallel produced identical results
assert bfs_s.distances == bfs_p.distances

// 4. Repeat for SSSP (source=0) and CC
// 5. Measure times, report speedup, write BFS.txt / SSSP.txt / CC.txt
```

## Testing & Verification

1. **Small test cases**: Build small graphs (5–10 vertices) where you can manually compute expected BFS distances, SSSP shortest paths, and CC labels. Include:
   - Disconnected components (for CC)
   - Cycles (to test convergence)
   - Varying edge weights (to distinguish SSSP from BFS)

2. **Serial-parallel consistency**: Verify that serial and parallel produce identical per-vertex results for all three algorithms.

3. **Provided dataset**: Download `soc-LiveJournal1-weighted.txt` from the [shared folder](https://drive.google.com/drive/folders/1MR0FuR0UxRCIFYBpEQ9e4Bx8s4qA_U7F?usp=sharing). This is a directed social network graph:
   - 4,847,571 vertices, 68,993,773 edges
   - Tab-separated, with integer edge weights in [1, 400]
   - Use `head -n 5 <file-name>` to have a quick look at the format of the provided dataset.
   - Add it (and the answer files) to `.gitignore` so it is not committed to GitHub.

4. **Answer files**: The shared folder also contains expected answer files (`BFS.txt`, `SSSP.txt`, `CC.txt`) for the provided dataset. Compare your output against these files to verify correctness. BFS and SSSP use source vertex 0.

## Experiments

Run the following experiments on `soc-LiveJournal1-weighted.txt` and report in your development journal. Use source vertex 0 for BFS and SSSP.

1. **Serial execution time** for BFS, SSSP, and CC.

2. **Speedup table**: For each algorithm, report:

   | Algorithm | Serial | 1 thread | 2 threads | 4 threads | 8 threads |
   |---|---|---|---|---|---|
   | BFS | | | | | |
   | SSSP | | | | | |
   | CC | | | | | |

### Discussion

Answer the following questions in your development journal:

1. Inside `Process(tid, v, g)`, which memory location does the function write to? Using the parallel runner's vertex partitioning, explain why two threads can never write to the same location in one round.

2. Compare the parallel speedup you measured for BFS, SSSP, and CC. Which algorithm scales best, and what property of the algorithm explains the difference?

3. In BFS, `dist_[v]` stores the current shortest hop distance from the source. BFS has two useful properties: (i) once `dist_[v]` is set, later rounds can never change it; (ii) in each round, v's newly-discovered in-neighbors all have the same distance. How could you use these facts to skip work? Why would the same tricks not work for SSSP or CC? (Implementing these optimizations is not required, but strongly encouraged: you should observe a significant speedup over the baseline BFS.)

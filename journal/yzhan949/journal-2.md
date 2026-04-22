## Name / NetID
Linda Zhang / yzhang949

---

## Session Date
April 21, 2026 (approximately 3–4 hours)

---

## Objective
Implement and verify the BSP-based framework for Phase 2a, including BFS, SSSP, and CC algorithms, and ensure that both serial and parallel executions produce identical results on a large-scale graph dataset.

---

## Attempts Made

During this session, I integrated the BSP framework with three graph algorithms (BFS, SSSP, and CC) using a CSR graph structure with both forward and reverse edges.

Initially, I encountered an issue where the graph was not loaded correctly, resulting in:

```cpp
verticles = 0;
Edges: 0;

```

This caused all algorithms to trivially pass correctness checks due to empty results. After investigation, I found that the issue was caused by an incorrect file path. Once corrected, the graph loaded properly:

```cpp
verticles = 4847571
edges = 7572941

```

I then implemented BFS, SSSP, and CC using the BSP model, ensuring that:

- Each `Process()` function reads only from `state_prev_`
- Each thread writes only to its assigned vertex
- Double buffering (`state_` and `state_prev_`) prevents race conditions

Example (BFS core logic):

```cpp
for (int i = g.in_offsets[v]; i < g.in_offsets[v + 1]; i++) {
    int u = g.in_edges[i];
    if (dist_prev_[u] == -1) continue;

    int candidate = dist_prev_[u] + 1;
    if (best == -1 || candidate < best) {
        best = candidate;
        updated = true;
    }
}
```

Both serial (BspSerial) and parallel (BspParallel) executions were tested.

## Solutions / Workarounds

To resolve the graph loading issue, I corrected the file path used in LoadGraph and verified edge parsing through debug output.

For correctness verification, I compared the results of serial and parallel executions:

```cpp
bool bfs_equal = (bfs_serial.GetDistances() == bfs_parallel.GetDistances());
```

All three algorithms produced identical results:

```cpp
BFS   serial=741 ms, parallel=533 ms, equal=YES
SSSP  serial=1907 ms, parallel=1643 ms, equal=YES
CC    serial=755 ms, parallel=687 ms, equal=YES
```

I also implemented output file generation for:

```cpp
BFS.txt
SSSp.txt
CC.txt
```

Each file follows the required format:
```cpp
vertex_id value
```

## Experiments / Analysis
All experiments were conducted on the dataset:
```cpp
soc-LiveJournal1-weighted.txt
```

Timing Results
```cpp
| Algorithm | Serial (ms) | Parallel (ms) | Speedup |
| --------- | ----------- | ------------- | ------- |
| BFS       | 741         | 533           | 1.39×   |
| SSSP      | 1907        | 1643          | 1.16×   |
| CC        | 755         | 687           | 1.10×   |
```

Analysis
```cpp
BFS achieved the highest speedup (~1.39×). It converges quickly and has relatively uniform workload distribution.

SSSP showed limited speedup (~1.16×). Its Bellman-Ford style relaxation requires many iterations and repeated full-graph scans.

CC had the lowest speedup (~1.10×). Label propagation converges slowly and involves frequent updates.
```
Overall, the parallel implementation improves performance, but gains are limited by memory access patterns and synchronization overhead.

## Learning Outcome

In this session, I learned:

```cpp
1. How the Bulk Synchronous Parallel (BSP) model organizes computation into rounds
2. The importance of double buffering (state_ and state_prev_) to avoid race conditions
3. How reverse CSR enables efficient pull-based graph processing
4. Differences in scalability among BFS, SSSP, and CC

```

This project improved my understanding of both parallel programming and graph algorithm design.

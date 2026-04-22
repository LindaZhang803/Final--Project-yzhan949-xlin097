## Name / NetID
Xinhui Lin / xlin097

---

## Session Date
April 21, 2026 (about 2.5 hours)

---

## Objective
My goal was to prepare the starting of this phase so my teammate can implement BFS, SSSP, and CC on top of it.

---

## Attempts Made
I focused on data structure and framework setup.

I updated the graph structure so it can support weighted edges and reverse edges.

Main work I did:
- Extended `CSRGraph` to include:
  - forward edge weights
  - reverse CSR arrays
- Updated `LoadGraph` to read weighted edge lines
- Built forward and reverse CSR
- Added a new BSP file and running file:
  - serial runner
  - parallel runner

Small code example used:

```cpp
while (algo.HasWork()) {
    for (int v = 0; v < g.num_vertices; v++) {
        algo.Process(0, v, g);
    }
    algo.PostRound();
}
```

---

## Solutions / Workarounds
One concern was working with old graph files. I handled this by using default weight when a third column is missing.

---

## Experiments / Analysis
This session was mainly a basic stuff so I did not run full tests yet.

---

## Learning Outcome
In this session I learned:
1. How to extend CSR for weighted and reverse edges
2. How to design a simple BSP

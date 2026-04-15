**Name/NetID**: Xinhui Lin / xlin097  

**Session Date**: 2026/4/14 (about 2 hours)  

**Objective**: Implement Phase 1 core functions. load graph from edge list, build CSR format, and run k hop query on small tests.  

**Attempts made:**

- I first used the `LoadGraph` function from the lab assignment week 2
- Then I built CSR arrays so neighbors can be accessed
- I implemented k hop for two query types:
	- count reachable nodes
	- find max reachable node id
- I tested with `queries20.txt` and checked each output

Small code example used:

```cpp
for (int i = g.offsets[u]; i < g.offsets[u + 1]; ++i) {
		int v = g.edges[i];
		if (!visited[v]) {
				visited[v] = 1;
				frontier.push_back(std::make_pair(v, depth + 1));
		}
}
```

Testing result:

- Build command worked: `g++ -std=c++17 -O2 code/src/*.cpp -Icode/head -o code/phase1`
- Run command worked: `./code/phase1 soc-Slashdot0902.txt queries20.txt`
- Correct queries: `20/20`

**Experiments/Analysis (if applicable)**:

- Main experiment was correctness checking on `queries20.txt`
- The program answered all 20 queries correctly
- This shows CSR loading and k hop query logic are working for small tests

**Learning outcome**: I learned how to convert an edge list graph into CSR format, how to run k hop search, and how to use small files to verify correctness before moving to larger tests

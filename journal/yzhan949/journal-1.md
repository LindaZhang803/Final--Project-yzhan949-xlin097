**Name/NetID**: Linda Zhang / yzhan949  

**Session Date**: 2026/4/14 (2 hours)  

---

## Objective

Implement the Phase 1 execution engine by completing three main components:
- sequential execution of query tasks
- parallel execution using multiple threads
- result consistency checking between sequential and parallel execution

The purpose of this session was to make sure that the same query workload could be executed correctly in both sequential and parallel modes.

---

## Attempts made

In this session, I worked on the execution layer in `parallel.cpp`. I first implemented the sequential version as the baseline, since it is the simplest way to verify correctness. In `RunTasksSequential`, each task is executed one by one, and the result is written back into the corresponding `QueryTask`.

```cpp
void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks) {
    for (auto& task : tasks) {
        task.result = task.cb(g, task.src, task.K);
    }
}
```
After that, I implemented the parallel version. The main challenge was dividing the task list safely across multiple threads. I needed to make sure that threads would not overwrite each other’s results. To solve this, I used a worker function that processes only a fixed range of tasks:

```cpp
void Worker(const CSRGraph& g, std::vector<QueryTask>& tasks, int start, int end) {
    for (int i = start; i < end; i++) {
        tasks[i].result = tasks[i].cb(g, tasks[i].src, tasks[i].K);
    }
}
```
Then, in RunTasksParallel, I split the tasks into chunks and assigned each chunk to one thread. I also handled invalid input and edge cases, such as num_threads <= 0 and an empty task list.

```cpp
void RunTasksParallel(const CSRGraph& g, std::vector<QueryTask>& tasks, int num_threads) {
    if (num_threads <= 0) {
        throw std::invalid_argument("num_threads must be > 0");
    }

    int n = static_cast<int>(tasks.size());
    if (n == 0) {
        return;
    }

    int actual_threads = std::min(num_threads, n);
    int chunk = (n + actual_threads - 1) / actual_threads;

    std::vector<std::thread> threads;
    threads.reserve(actual_threads);

    for (int t = 0; t < actual_threads; t++) {
        int start = t * chunk;
        int end = std::min(start + chunk, n);

        if (start >= n) {
            break;
        }

        threads.emplace_back(Worker, std::cref(g), std::ref(tasks), start, end);
    }

    for (auto& th : threads) {
        th.join();
    }
}
```
One difficulty during this process was making sure that the number of threads did not exceed the number of tasks. If too many threads are created, some threads would do no useful work. I addressed this by using:

```cpp
int actual_threads = std::min(num_threads, n);
```
Another issue was correctness checking. Since parallel execution may appear to run successfully even when results are wrong, I implemented CheckResultsEqual to compare outputs from sequential and parallel executions.

```cpp
bool CheckResultsEqual(const std::vector<QueryTask>& a, const std::vector<QueryTask>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].result != b[i].result) {
            return false;
        }
    }

    return true;
}
```
This gave me a simple way to validate that the parallel engine produced the same results as the sequential baseline.


## Solutions / Workarounds

To make the implementation correct and manageable, I used the following approaches:

- Treated sequential execution as the reference implementation  
- Used chunk-based partitioning so each thread works on a separate subrange of the task vector  
- Limited the number of threads using `std::min(num_threads, n)` to avoid unnecessary overhead  
- Added input validation to ensure `num_threads > 0`  
- Used `join()` to synchronize all worker threads before returning  
- Implemented result comparison to verify correctness after execution  

A key workaround was ensuring that each thread only writes to its own assigned task indices. This avoids race conditions and eliminates the need for locks or mutexes.

### Testing Result

I tested both sequential and parallel implementations using the same set of `QueryTask`s and compared their outputs.

**Expected behavior:**
- Sequential execution completes normally  
- Parallel execution completes normally  
- `CheckResultsEqual(...)` returns `true`  

**Observed behavior:**
- Both versions completed successfully  
- Results matched across all tested tasks  
- Invalid thread counts correctly triggered exceptions  

### Relevant Github Commits

- Implemented `RunTasksSequential`  
- Implemented `RunTasksParallel` with worker threads  
- Added `CheckResultsEqual` for validation  

---

## Experiments / Analysis

To verify correctness, I compared outputs from sequential and parallel executions on identical inputs. The results matched exactly, confirming correctness of the parallel implementation.

Key observations:

- Sequential execution is simpler and easier to debug  
- Parallel execution improves scalability but introduces complexity  
- Chunk-based scheduling is effective when workload is balanced  
- Writing to disjoint regions avoids synchronization overhead  

An important insight is that multithreading requires careful workload partitioning. Even if a program runs successfully, incorrect partitioning can silently produce wrong results.

---

## Learning Outcome

- Learned how to design a sequential execution baseline  
- Learned how to use `std::thread` for parallel execution  
- Understood how to partition workloads across threads  
- Learned how to avoid race conditions without using locks  
- Learned how to synchronize threads using `join()`  
- Learned how to validate correctness via result comparison  
- Improved understanding of concurrency and execution correctness  

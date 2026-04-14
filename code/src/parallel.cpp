#include "../head/parallel.h"

#include <thread>
#include <algorithm>
#include <stdexcept>

namespace {

void Worker(const CSRGraph& g, std::vector<QueryTask>& tasks, int start, int end) {
    for (int i = start; i < end; i++) {
        tasks[i].result = tasks[i].cb(g, tasks[i].src, tasks[i].K);//调用查询回调函数并存储结果
    }
}//匿名函数

}  // namespace

void RunTasksSequential(const CSRGraph& g, std::vector<QueryTask>& tasks) {
    for (auto& task : tasks) {
        task.result = task.cb(g, task.src, task.K);//调用查询回调函数并存储结果
    }
}

void RunTasksParallel(const CSRGraph& g, std::vector<QueryTask>& tasks, int num_threads) {
    if (num_threads <= 0) {
        throw std::invalid_argument("num_threads must be > 0");//检查线程数是否合法
    }

    int n = static_cast<int>(tasks.size());
    if (n == 0) {
        return;//没有任务需要处理
    }

    int actual_threads = std::min(num_threads, n);//实际使用的线程数不能超过任务数
    int chunk = (n + actual_threads - 1) / actual_threads;//计算每个线程处理的任务数量，向上取整

    std::vector<std::thread> threads;//创建线程池
    threads.reserve(actual_threads);//预留线程池空间

    for (int t = 0; t < actual_threads; t++) {
        int start = t * chunk;
        int end = std::min(start + chunk, n);

        if (start >= n) {
            break;//如果起始索引超过任务数量，停止创建线程
        }

        threads.emplace_back(Worker, std::cref(g), std::ref(tasks), start, end);//创建线程并分配任务范围
    }

    for (auto& th : threads) {
        th.join();//等待所有线程完成
    }
}

bool CheckResultsEqual(const std::vector<QueryTask>& a, const std::vector<QueryTask>& b) {
    if (a.size() != b.size()) {
        return false;//如果两个任务列表大小不同，结果不相等
    }

    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].result != b[i].result) {
            return false;//如果对应任务的结果不同，结果不相等
        }
    }

    return true;//所有任务结果都相等，返回true
}
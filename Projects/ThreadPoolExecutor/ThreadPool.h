#include <queue>

class ThreadPool {
 private:
  int active_threads = 0;
  bool stop = false;
  condition_variable cv;
  mutex queue_mutex;
  vector<thread> threads;
  queue<function<void()>> tasks;

 public:
  ThreadPool(int num_threads) {
    for (int i = 0; i < num_threads; i++) {
      threads.push_back(thread([this, i] {
        while (true) {
          function<void()> task;
          {
            unique_lock<mutex> lock(queue_mutex);

            cv.wait(lock, [this, i] { return !tasks.empty() || stop; });

            if (tasks.empty() || stop) return;

            task = move(tasks.front());
            // Make sure to increment counter within the same critical section
            // as consuming the task from the queue
            active_threads++;
            tasks.pop();
          }

          task();

          {
            unique_lock<mutex> lock(queue_mutex);
            active_threads--;
          }
        }
      }));
    }
  }

  template <typename F, typename... Args>
  auto enqueue(F&& f, Args&&... args) -> future<TaskResult> {
    auto task = make_shared<packaged_task<TaskResult()>>(
        [f = forward<F>(f), ... args = forward<Args>(args)]() {
          return TaskResult(f(args...));
        });
    auto result = task->get_future();

    {
      unique_lock<mutex> lock(queue_mutex);
      tasks.push([task]() { (*task)(); });
    }

    cv.notify_one();
    return result;
  }
  // Thread pool is in pending state until all tasks have been consumed and
  // completed
  bool pending() { return !tasks.empty() || active_threads > 0; }
  ~ThreadPool() {
    {
      unique_lock<mutex> lock(queue_mutex);
      stop = true;
    }
    cv.notify_all();
    for (auto& thread : threads) {
      thread.join();
    }
  }
};
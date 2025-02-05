#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

// Simple thread pool executor. A pool of workers consumes tasks from a queue
// The tasks use a shared console printer mutex to dictate the console output.
// The class exposes a pending method to show whether all tasks have been
// processed

class ThreadPool {
 private:
  int active_threads = 0;
  bool stop = false;
  condition_variable cv;
  mutex queue_mutex;
  vector<thread> threads;
  queue<function<void()> > tasks;

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
  void enqueue(function<void()> task) {
    {
      unique_lock<mutex> lock(queue_mutex);
      tasks.push(task);
    }
    cv.notify_one();
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

class Printer {
 public:
  static mutex mtx;
};

mutex Printer::mtx;

void main() {
  ThreadPool tp(5);

  for (int i = 0; i < 100; i++) {
    tp.enqueue([i] {
      // Use printer mutex to dictate output
      lock_guard<mutex> lock(Printer::mtx);

      cout << "Task " << i << " is running on " << this_thread::get_id()
           << endl;
    });
  }
  while (tp.pending()) {
  }
  cout << "Done" << endl;  // Illustrates that all threads finished computation
                           // before program termination
}
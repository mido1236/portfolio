#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Printer.h"
#include "TaskResult.h"
#include "ThreadPool.h"

using namespace std;

// Simple thread pool executor project. A pool of workers consumes tasks from a
// queue The tasks use a shared console printer mutex to dictate the console
// output. The class exposes a pending method to show whether all tasks have
// been processed

int main() {
  ThreadPool tp(100);
  vector<future<TaskResult>> futures;

  auto start_time = chrono::system_clock::now();

  for (int i = 0; i < 100; i++) {
    auto task = tp.enqueue(
        [i](int a, int b) {
#ifdef VERBOSE
          // Use printer mutex to dictate output
          unique_lock<mutex> lock(Printer::mtx);

          cout << "Task " << i << " is running on " << this_thread::get_id()
               << endl;
          lock.unlock();
#endif

          this_thread::sleep_for(chrono::milliseconds(150));
          return a + b;
        },
        i + 1, i + 2);

    // Note the use of move here, since future objects are not copyable
    futures.push_back(move(task));
  }

  for (int i = 0; i < futures.size(); i++) {
    auto result = futures[i].get();
    lock_guard<mutex> lock(Printer::mtx);

    cout << "Result " << i << ":" << result << endl;
  }

  cout << "Done" << endl;  // Illustrates that all threads finished computation
                           // before program termination

  // Show execution time
  cout << "Total time: "
       << chrono::duration_cast<chrono::milliseconds>(
              chrono::system_clock::now() - start_time)
       << endl;
  return 0;
}

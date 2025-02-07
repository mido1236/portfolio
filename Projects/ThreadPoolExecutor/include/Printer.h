#include <mutex>

using namespace std;

class Printer {
 public:
  static mutex mtx;
};

mutex Printer::mtx;
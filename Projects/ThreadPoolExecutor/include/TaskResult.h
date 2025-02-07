#include <any>

using namespace std;

class TaskResult {
    any result;

public:
    TaskResult() = default;

    template<typename T>
    explicit TaskResult(T value) : result(value) {
    }

    template<typename T>
    T get() {
        return any_cast<T>(result);
    }

    string toString() const {
        if (!result.has_value()) return "No Result";

        // Handle common types explicitly
        if (result.type() == typeid(int)) {
            return to_string(any_cast<int>(result));
        }
        if (result.type() == typeid(double)) {
            return to_string(any_cast<double>(result));
        }
        if (result.type() == typeid(string)) {
            return any_cast<string>(result);
        }

        return "[Unsupported Type]";
    }
};

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void foo(vector<int> &nums, size_t i) {
    nums[i] += 1;
    this_thread::sleep_for(chrono::seconds(2));
}

int main() {
    const size_t THREAD_AMOUNT = thread::hardware_concurrency();
    thread executors[THREAD_AMOUNT];
    vector<int> nums(THREAD_AMOUNT);
    for (size_t j = 0; j < 5; j++) {
        size_t i = 0;
        for (auto & executor : executors) {
            if (!executor.joinable()) {
                executor = thread(foo, ref(nums), i);
            }
            i += 1;
        }

        for (auto & executor : executors) {
            executor.join();
        }

        for (const auto &num : nums) {
            cout << num << "  ";
        }
        cout << endl;
    }


    return 0;
}




#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>
#include <cstdio>
#include <string>



void foo(std::vector<int> &nums, size_t i) {
    nums[i] += 1;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void threadTest() {
    const size_t THREAD_AMOUNT = std::thread::hardware_concurrency();
    std::thread executors[THREAD_AMOUNT];
    std::vector<int> nums(THREAD_AMOUNT);
    for (size_t j = 0; j < 1; j++) {
        size_t i = 0;
        for (auto & executor : executors) {
            if (!executor.joinable()) {
                executor = std::thread(foo, std::ref(nums), i);
            }
            i += 1;
        }

        for (auto & executor : executors) {
            executor.join();
        }

        for (const auto &num : nums) {
            std::cout << num << "  ";
        }
        std::cout << std::endl;
    }
}

int AMOUNT_OF_THREADS = 0;

void get_files_and_cataloges(const std::string &root, std::vector<std::string> &files, std::vector<std::string> &catalogs) {
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "/*").c_str(), &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            std::string fileName = std::string(f.cFileName);
            if (fileName != "." && fileName != "..") {
                if(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    catalogs.push_back(fileName);
                }
                else {
                    files.push_back(fileName);
                }
            }
        } while(FindNextFile(h, &f));
    }
    else {
        fprintf(stderr, "Error opening directory\n");
    }
};


void findFile(const std::string &root, const std::string &finding) {
    std::vector<std::string> files;
    std::vector<std::string> catalogs;
    get_files_and_cataloges(root, files, catalogs);

    for (const auto &f : files) {
        if (f == finding) std::cout << "Founded file: " << root << "/" << f << std::endl;
    }

    for (const auto &c : catalogs) {
        findFile(root + "/" + c, finding);
    }
}

int main() {
    // Подключаем русский язык для файлов с названием на русском
    setlocale(LC_ALL, "rus");

    std::string root = "C:/all";
    std::string finding = "main.py";

    findFile(root, finding);

    return 0;
}




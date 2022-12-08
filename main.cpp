#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>
#include <cstdio>
#include <string>
#include <stack>

int AMOUNT_OF_THREADS_NOW = 0;
bool IS_FOUND = false;

void get_files_and_catalogs(const std::string &root, std::vector<std::string> &files, std::vector<std::string> &catalogs);
void add_thread(const std::string &root, const std::string &finding, int max_thread);
void find_file(const std::string &root, const std::string &finding, int max_thread);
bool is_path_valid(const std::string &root);
void convert(std::string &root);

int main(int argc, char* argv[]) {
    // Подключаем русский язык для файлов с названием на русском
    setlocale(LC_ALL, "rus");

    std::string root = "C:/";
    std::string finding;
    int max_thread = 10;

    // Допустимое количество параметров
    if (argc == 2 || argc == 4 || argc == 6) {
        finding = argv[1];
        for (int i = 2; i < argc; i += 2) {
            std::string option = argv[i];
            std::string parameter = argv[i + 1];
            if (option == "--path") {
                root = parameter;
                convert(root);
            }
            else if (option == "--num_threads") max_thread = std::stoi(parameter);
            else {
                fprintf(stderr, "%s %s", "There is now such option", option.c_str());
                return -1;
            }
        }
    }
        // Недопустимое количество параметров
    else {
        fprintf(stderr, "Not valid. Use on of the following:\nfile_name.exe <path>\nfile_name.exe <file name> --path <path> --num_threads <thread amount>");
        return -1;
    }

    // Если путь некорректен, заканчиваем выполнение программы
    if (!is_path_valid(root)) {
        return -1;
    }

    find_file(root, finding, max_thread);
    while (AMOUNT_OF_THREADS_NOW > 0) {
        // К этому моменту не все потоки могут завершиться. Терпеливо ждем их завершения
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (!IS_FOUND) {
        // Если файл не найден, выводим сообщение об этом
        std::cout << "There is no file with this name" << std::endl;
    }

    return 0;
}


void get_files_and_catalogs(const std::string &root, std::vector<std::string> &files, std::vector<std::string> &catalogs) {
    // Функция, считывающая, какие и есть каталоги и файлы в каталоге root
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "*").c_str(), &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            std::string fileName = std::string(f.cFileName);
            if (fileName != "." && fileName != "..") {
                // Данная проверка позволяет отфильтровать файлы от каталогов
                if(f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    catalogs.push_back(fileName);
                }
                else {
                    files.push_back(fileName);
                }
            }
        } while(FindNextFile(h, &f));
    }
};


void add_thread(const std::string &root, const std::string &finding, int max_thread) {
    // Добавление одного треда
    AMOUNT_OF_THREADS_NOW++;
    find_file(root, finding, max_thread);
    AMOUNT_OF_THREADS_NOW--;
}

void find_file(const std::string &root, const std::string &finding, int max_thread) {
    // Основная функция для поиска файла
    if (IS_FOUND) return;
    std::stack<std::string> stack_catalogs;
    stack_catalogs.push(root);

    while (!stack_catalogs.empty()) {
        std::string temp_root = stack_catalogs.top();
        stack_catalogs.pop();

        std::vector<std::string> files;
        std::vector<std::string> catalogs;

        get_files_and_catalogs(temp_root, files, catalogs);
        // В цикле перебираем файлы. Если нашли - выводим найденный файл и заканчиваем выполнение функции
        for (int i = 0; i < files.size(); i++) {
            if (files[i] == finding && !IS_FOUND) {
                IS_FOUND = true;
                std::string result = "Founded file: " + temp_root + files[i] + "\n";
                std::cout << result;
                return;
            }
        }

        // Перебираем каталоги. Если есть возможность выделить тред - делаем это. Иначе продолжаем считать в этой же функции
        for (int i = 0; i < catalogs.size(); i++) {
            if (AMOUNT_OF_THREADS_NOW < max_thread) {
                std::thread thr(add_thread, temp_root + catalogs[i] + "/", finding, max_thread);
                thr.detach();
            }
            else {
                stack_catalogs.push(temp_root + catalogs[i] + "/");
            }
        }
    }
}

bool is_path_valid(const std::string &root) {
    // Проверка, корректно ли написан путь и есть ли по этому пути папка
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "*").c_str(), &f);
    if(h == INVALID_HANDLE_VALUE || root[root.size() - 1] != '/') {
        fprintf(stderr, "Path is not valid. Use for example C:/Program Files/");
        return false;
    }
    return true;
}

void convert(std::string &root) {
    // Функция, которая меняет символы '\' в '/'
    for (int i = 0; i < root.size(); i++) {
        if (root[i] == '\\') root[i] = '/';
    }
}




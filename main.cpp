#include <iostream>
#include <thread>
#include <vector>
#include <windows.h>
#include <cstdio>
#include <string>
#include <queue>
using namespace std;
bool IS_FOUND = false;

struct Tree {
    string name;
    string full_name;
    vector<Tree*> childs;
    vector<string> files;
    void add_child(vector<string> &files_, vector<string> &catalogs) {
        files = files_;
        for (int i = 0; i < catalogs.size(); i++) {
            childs.push_back(new Tree);
            childs[i]->full_name = full_name + "/" + catalogs[i];
            childs[i]->name = catalogs[i];
        }
    }
};

int AMOUNT_OF_THREADS_NOW = 0;

void add_thread(Tree &tree, const string &finding, int max_thread);
void clear_tree(Tree &tree);
void get_files_and_catalogs(const string &root, vector<string> &files, vector<string> &catalogs);
void build_tree(const string &root, Tree &tree);
void find_file(Tree &tree, const string &finding, int max_thread);
bool is_path_valid(const string &root);
void convert(string &root);

int main(int argc, char* argv[]) {
    // Подключаем русский язык для файлов с названием на русском
    setlocale(LC_ALL, "rus");

    string root = "C:";

    string finding;
    int max_threads = 10;

    //    // Допустимое количество параметров
    if (argc == 2 || argc == 4 || argc == 6) {
        finding = argv[1];
        for (int i = 2; i < argc; i += 2) {
            string option = argv[i];
            string parameter = argv[i + 1];
            if (option == "--path") {
                root = parameter;
                convert(root);
            }
            else if (option == "--num_threads") max_threads = stoi(parameter);
            else {
                fprintf(stderr, "%s %s", "There is now such option", option.c_str());
                return -1;
            }
        }
    }
        // Недопустимое количество параметров
    else {
        fprintf(stderr, "Not valid. Use one of the following:\nfind_app.exe <file name>\nfind_app.exe <file name> --path <path> --num_threads <thread amount>");
        return -1;
    }

    // Если путь некорректен, заканчиваем выполнение программы
    if (!is_path_valid(root)) {
        return -1;
    }

    Tree tree;
    tree.full_name = root;
    cout << "Building tree..." << endl;
    build_tree(root, tree);
    cout << "Tree is built..." << endl;
    find_file(tree, finding, max_threads);

    while (AMOUNT_OF_THREADS_NOW > 0) {
        // К этому моменту не все потоки могут завершиться. Терпеливо ждем их завершения
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    if (!IS_FOUND) {
        // Если файл не найден, выводим сообщение об этом
        cout << "There is no file with this name" << endl;
    }


    clear_tree(tree);

    return 0;
}

void get_files_and_catalogs(const string &root, vector<string> &files, vector<string> &catalogs) {
    // Функция, считывающая, какие и есть каталоги и файлы в каталоге root
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "/*").c_str(), &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            string fileName = string(f.cFileName);
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

void build_tree(const string &root, Tree &tree) {
    queue<string> queue_catalogs;
    queue<Tree*> queue_tree;
    queue_catalogs.push(root);
    queue_tree.push(&tree);


    while (!queue_tree.empty()) {
        string temp_root = queue_catalogs.front();
        Tree* temp_tree = queue_tree.front();
        queue_catalogs.pop();
        queue_tree.pop();

        vector<string> files;
        vector<string> catalogs;

        get_files_and_catalogs(temp_root, files, catalogs);

        temp_tree->add_child(files, catalogs);

        for (auto & child : temp_tree->childs) {
            queue_tree.push(child);
            queue_catalogs.push(child->full_name);
        }
    }
}

void add_thread(Tree &tree, const string &finding, int max_thread) {
    // Добавление одного треда
    AMOUNT_OF_THREADS_NOW++;
    find_file(tree, finding, max_thread);
    AMOUNT_OF_THREADS_NOW--;
}

void find_file(Tree &tree, const string &finding, int max_thread) {
    // Основная функция для поиска файла
    if (IS_FOUND) return;

    queue<Tree*> queue_tree;
    queue_tree.push(&tree);


    while (!queue_tree.empty()) {
        Tree* temp_tree = queue_tree.front();
        queue_tree.pop();

        for (const auto &f : temp_tree->files) {
            if (f == finding && !IS_FOUND) {
                IS_FOUND = true;
                cout << "Founded file: " + temp_tree->full_name + "/" + f << endl;
                return;
            }
        }

        for (auto & child : temp_tree->childs) {
            if (AMOUNT_OF_THREADS_NOW < max_thread) {
                thread thr(add_thread, ref(*child), finding, max_thread);
                thr.detach();
            }
            else {
                queue_tree.push(child);
            }
        }

        if (IS_FOUND) return;
    }
}

bool is_path_valid(const string &root) {
    // Проверка, корректно ли написан путь и есть ли по этому пути папка
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "*").c_str(), &f);
    if(h == INVALID_HANDLE_VALUE || root[root.size() - 1] == '/') {
        fprintf(stderr, "Path is not valid. Use for example C:/Program Files");
        return false;
    }
    return true;
}

void convert(string &root) {
    // Функция, которая меняет символы '\' в '/'
    for (char &ch : root) {
        if (ch == '\\') ch = '/';
    }
}

void clear_tree(Tree &tree) {
    for (auto & child : tree.childs) {
        clear_tree(*child);
        delete child;
    }
}




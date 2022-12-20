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

void clear_tree(Tree &tree);
void get_files_and_catalogs(const string &root, vector<string> &files, vector<string> &catalogs);
void build_tree(const string &root, Tree* tree);
void find_file(const string &root, const string &finding, int max_thread);
bool is_path_valid(const string &root);
void convert(string &root);

int main(int argc, char* argv[]) {
    // Подключаем русский язык для файлов с названием на русском
    setlocale(LC_ALL, "rus");

//    string root = "C:";
    string root = "C:";

    string finding;
    int max_thread = 10;

    Tree tree;
    tree.full_name = root;
    cout << "Building tree..." << endl;
    build_tree(root, &tree);
    cout << "Tree was built..." << endl;


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

void build_tree(const string &root, Tree* tree) {
    queue<string> queue_catalogs;
    queue<Tree*> queue_tree;
    queue_catalogs.push(root);
    queue_tree.push(tree);

    while (!queue_tree.empty()) {
        string temp_root = queue_catalogs.front();
        Tree* temp_tree = queue_tree.front();
        queue_catalogs.pop();
        queue_tree.pop();

        vector<string> files;
        vector<string> catalogs;

        get_files_and_catalogs(temp_root, files, catalogs);

        temp_tree->add_child(files, catalogs);

        for (int i = 0; i < temp_tree->childs.size(); i++) {
            queue_tree.push(temp_tree->childs[i]);
            queue_catalogs.push(temp_tree->childs[i]->full_name);
        }
    }
}

bool is_path_valid(const string &root) {
    // Проверка, корректно ли написан путь и есть ли по этому пути папка
    WIN32_FIND_DATA f;

    HANDLE h = FindFirstFile((root + "*").c_str(), &f);
    if(h == INVALID_HANDLE_VALUE || root[root.size() - 1] != '/') {
        fprintf(stderr, "Path is not valid. Use for example C:/Program Files/");
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
    for (int i = 0; i < tree.childs.size(); i++) {
        clear_tree(*tree.childs[i]);
        delete tree.childs[i];
    }
}




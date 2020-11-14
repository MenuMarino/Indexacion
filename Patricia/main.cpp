#include "Patricia.h"
#include <ctime>
#include <random>
#include <chrono>
#include <unistd.h>

using namespace std;

const string FILENAME = "file.db";
const int FILENAME_LINE_COUNT = 3386;

void stressTest() {
    Patricia* patricia = new Patricia();
    ifstream keysFile("keys.db");
    string line;
    vector<string> keys;

    patricia->build(FILENAME);

    for(int i = 0; i < 100; i++) {
        auto index = rand() % FILENAME_LINE_COUNT;
        while(index--) getline(keysFile,line);
        keysFile.seekg(0,ios::beg);
        keys.push_back(line);
    }
    cout<<"\n\n\n";
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < 100; i++) {
        printf("Searching: %s\n", keys[i].c_str());
        auto test1 = patricia->search(keys[i]);
        printf("Occurrences: %d\n\n", (int)test1.size());
        printf("Path of each occurrence:\n");
        for (auto& path : patricia->getPaths(keys[i])) {
            cout << path << "\n";
        }
        printf("Searching prefix: %s\n", keys[i].c_str());
        auto test2 = patricia->prefix(keys[i]);
        printf("Word cnt with given prefix: %d\n\n", (int)test2);
    }
    auto end = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "\n===========================\n\n";
    cout << "Execution time: " << executionTime.count() << " us.\n";
    cout << "Read count (avg): " << SEARCH_COUNTER/100 << "\n";
}

int main(){

    srand(time(nullptr));

    cout << getpid() << endl;
    // string command = "top | grep " + to_string(getpid()) + " | awk '{print $8}'";
    // cout << command << endl;

    // system(command.c_str());

    // top | grep 9442 & sleep 10 && kill $!

    // sleep(100);

    // return 0;

    Patricia* patricia = new Patricia();

    patricia->build(FILENAME);

    cout << "word with prefix:\n";
    cout << patricia->prefix("Licens") << "\n";

    for (auto& path : patricia->getPaths("License")) {
        cout << path << "\n";
    }

    stressTest();

    delete patricia;

    return 0;
}
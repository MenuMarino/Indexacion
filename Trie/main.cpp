#include "Trie.h"
#include <ctime>
#include <random>
#include <chrono>
#include <unistd.h>

using namespace std;

const string FILENAME = "file.db";

void stressTest() {
    Trie* trie = new Trie();
    string line;
    ifstream keysFile("keys.db");
    vector<string> keys;

    trie->build(FILENAME);

    for(int i = 0; i < 100; i++) {
        auto index = rand() % 3386;
        while(index--) getline(keysFile,line);
        keysFile.seekg(0,ios::beg);
        keys.push_back(line);
    }
    cout<<"\n\n\n";
    auto start = chrono::high_resolution_clock::now();
    for(int i = 0; i < 100; i++) {
        printf("Searching: %s\n", keys[i].c_str());
        auto test1 = trie->search(keys[i]);
        printf("Occurrences: %d\n\n", (int)test1.size());
        printf("Path of each occurrence:\n");
        for (auto& path : trie->getPaths(keys[i])) {
            cout << path << "\n";
        }
        printf("Searching prefix: %s\n", keys[i].c_str());
        auto test2 = trie->prefix(keys[i]);
        printf("Word cnt with given prefix: %d\n\n", (int)test2);
    }
    auto end = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "\n===========================\n\n";
    cout << "Execution time: " << executionTime.count() << " us.\n";
    cout << "Read count: " << SEARCH_COUNTER/100 << "\n";
    delete trie;
}

int main(){

    srand(time(nullptr));

    Trie* trie = new Trie();

    trie->build(FILENAME);

    stressTest();

    cout << trie->prefix("Licens") << "\n";

    return 0;
}
#include <iostream>
#include <vector>
#include <chrono>
#include "ternaryTree.hpp"

using namespace std;

const string filename = "file.db";

int main() {
    TernaryTree tt;

    auto start = chrono::high_resolution_clock::now();
    auto todasLasKeys = tt.build(filename);
    auto end = chrono::high_resolution_clock::now();
    auto executionTime = chrono::duration_cast<chrono::milliseconds>(end - start);
    auto buildTime = executionTime.count();
    
    
    ifstream file(filename, ios::binary);

    vector<string> keys (todasLasKeys.begin(), todasLasKeys.begin() + 100);
    
    start = chrono::high_resolution_clock::now();
    for (auto i : keys) {
        // No repite palabras en el search
        cout << "===================" << '\n';
        auto response = tt.search(i);
        cout << "Key: " << i << " tiene " << response.size() << " repeticiones.\n";
        for (auto direccion_offset : response) {
            file.seekg(direccion_offset.first, ios::beg);
            char* buffer = new char[direccion_offset.second - 1];
            file.read(buffer, direccion_offset.second - 1);
            cout << " * " << buffer << '\n';
        }
        cout << "===================\n\n" << '\n';
    }
    end = chrono::high_resolution_clock::now();
    executionTime = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Archivos que empiezan con Power: \n";
    tt.prefix("Power");

    cout << "\nKeys: " << keys.size() << ".\n";
    cout << "Build: " << buildTime << " ms.\n";
    cout << "Search all keys: " << executionTime.count() << " ms.\n";
    cout << "Reads (promedio): " << reads/keys.size() << ".\n";

    file.close();
    return 0;
}
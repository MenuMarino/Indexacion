#include <string>
#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include "Node.h"

using namespace std;
unsigned long long SEARCH_COUNTER = 0;
typedef pair<char,Node*> child;

string getFileNameFromRoute(string s) {
     string delimiter = "/";

     size_t pos = 0;
     string token;
     while ((pos = s.find(delimiter)) != string::npos) {
          token = s.substr(0, pos);
          // cout << token << "\n";
          s.erase(0, pos + delimiter.length());
     }

     // cout << s << "\n";

     if ((pos = s.find('.')) != string::npos) {
          token = s.substr(0, pos);
          return token;
     }
     return "error";
}

class Trie{
private:
    Node* root;
    
public:
    Trie();
    ~Trie();

    Node* getRoot(){return root;}

    void insert(string word, unsigned int pdir, unsigned int offset);
    void deleteWord(string word);
    int prefix(string word);
    vector<pair<unsigned int, unsigned int>> search(string word);
    void print();
    void print(Node* node, int l);
    void build(string file);
    void patricia();
    int count(Node*);
    vector<string> getPaths(string word);
};

Trie::Trie(){
    root = new Node();
}

Trie::~Trie(){
}

void Trie::insert(string word, unsigned int pdir, unsigned int offset){
    Node* cur = root;
    int index = 0;
    while(index < word.size()){
        if(cur->exists(word[index])){//key found
            cur = (*cur)[word[index]].second;
        }
        else{
            //cout<<"Created new node for: "<<word[index]<<"\n";
            cur->children.push_back(child(word[index],new Node()));
            cur = (*cur)[word[index]].second;
        }
        index++;
    }
    cur->state = true;
    //set pdir, offset
    cur->filePos.push_back({pdir, offset});
}

void Trie::deleteWord(string word){
    Node* cur = root;
    int index = 0;
    stack<Node*> route;
    while(index < word.size()){
        if(cur->exists(word[index])){//key found
            route.push(cur);
            cur = (*cur)[word[index]].second;
        }
        else{
            //cout<<"Word not found.\n";
            return;
        }
        index++;
    }

    index--;
    cur->state = false;
    cur->filePos.clear();

    while(!route.empty()){
        if(cur->state == false && cur->children.size() == 0){
            //cout<<"Deleting link to "<<word[index]<<"\n";
            delete cur;
            cur = route.top();
            route.pop();
            cur->erase(word[index]);
            index--;
        }
        if(cur->children.size() != 0){
            return;
        }
    }
    
}

vector<string> Trie::getPaths(string word) {
    auto filePositions = search(word);
    vector<string> paths;
    ifstream file("file.db");

    if (!filePositions.empty() && file.is_open()) {
        string path;
        for (auto& par : filePositions) {
            file.seekg(par.first, ios::beg);
            getline(file, path);
            paths.push_back(path);
        }
    }
    return paths;
}

int Trie::count(Node* x) {
    int cnt = x->filePos.size();
    for (auto c : x->children) {
        cnt += count(c.second);
    }
    return cnt;
}

int Trie::prefix(string word){
    Node* cur = root;
    int index = 0;
    while(index < word.size()){
        if(cur->exists(word[index])){//key found
            cur = (*cur)[word[index]].second;
        }
        else{
            return false;
        }
        index++;
    }
    return count(cur);
}

vector< pair<unsigned int, unsigned int> > Trie::search(string word){
    Node* cur = root;
    int index = 0;
    while(index < word.size()){
        SEARCH_COUNTER++;
        if(cur->exists(word[index])){//key found
            
            cur = (*cur)[word[index]].second;
        }
        else{
            return vector<pair<unsigned int, unsigned int>>();
        }
        index++;
    }
    return cur->filePos;
}

void Trie::print(){
    if(!root){
        cout<<"Empty tree.\n";
        return;
    }
    Node* node = root;
    for(const auto pair: node->children){
        cout<<pair.first<<"\n";
        print((*node)[pair.first].second,1);
    }

}

void Trie::print(Node* node, int l){
    for(const auto pair: node->children){
        for(int i = 0; i < l; i++) cout<<"\t";
        cout<<pair.first<<"\n";
        print((*node)[pair.first].second, l+1);
    }
}

void Trie::build(string filename){ 
    ifstream file(filename);
    ofstream keys("keys.db");
    string line, key;
    unsigned int pgdir = 0, offset;
    while(getline(file,line)) {
        offset = (unsigned int)file.tellg() - pgdir;
        key = getFileNameFromRoute(line);
        keys<<key<<"\n";
        insert(key, pgdir, offset);
        pgdir = file.tellg();
    }
    file.close();
    keys.close();
}
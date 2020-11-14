#include <string>
#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include <algorithm>
#include "Node.h"

using namespace std;
unsigned long long SEARCH_COUNTER = 0;
typedef pair<char,Node*> child;
bool DEBUG_MODE = false;

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

     if ((pos = s.find_last_of('.')) != string::npos) {
          token = s.substr(0, pos);
          return token;
     }
     return "error";
}

class Patricia{
private:
    Node* root;
    
public:
    Patricia();
    ~Patricia();

    Node* getRoot(){return root;}

    void insert(string word, unsigned int pdir, unsigned int offset);
    void deleteWord(string word);
    size_t prefix(string word);
    vector<pair<unsigned int, unsigned int>> search(string word);
    void print();
    void print(Node* node, int l);
    void build(string file);
    void patricia();
    void pNode(Node* n);
    size_t count(Node* x);
    vector<string> getPaths(string word);
};

Patricia::Patricia(){
    root = new Node();
}

Patricia::~Patricia(){
}

void Patricia::insert(string word, unsigned int pdir, unsigned int offset){
    Node *cur = root, *parent;
    int index = 0;

    while(true) {
        //cout<<"insertloop\n";
        bool notFound = true;
        for(auto p: cur->children) {
            if(p.first[0] == word[index]) {
                notFound = false;
                int matchIndex = 1;
                index++;
                while(matchIndex < p.first.size() && index < word.size() && p.first[matchIndex] == word[index]) {
                    if(DEBUG_MODE) cout<<"1\n";
                    matchIndex++;
                    index++;
                }
                if(index < word.size() && matchIndex == p.first.size()) {
                    if(DEBUG_MODE) cout<<"2\n";
                    parent = cur;
                    cur = p.second;
                    break;
                }
                else if(index == word.size() && matchIndex == p.first.size()) {
                    if(DEBUG_MODE) cout<<"3\n";
                    cur = p.second;
                    cur->state = true;
                    cur->filePos.push_back({pdir, offset});
                    return;
                }
                else if(matchIndex == p.first.size()) {
                    if(DEBUG_MODE) cout<<"5\n";
                    cur->children.push_back({word.substr(index),new Node(true)});
                    cur = (*cur)[word.substr(index)].second;
                    cur->filePos.push_back({pdir, offset});
                    return;
                }
                else if(index <= word.size() && matchIndex < p.first.size()){
                    if(DEBUG_MODE) cout<<"4*\n";
                    Node* temp = new Node(index == word.size());
                    temp->children.push_back({p.first.substr(matchIndex),p.second});
                    temp->children.push_back({word.substr(index),new Node(true)});
                    auto it = find_if(cur->children.begin(), cur->children.end(), [&] (const pair<string,Node*>& e) {return p.first == e.first;});
                    cur->children.erase(it);
                    cur->children.push_back({p.first.substr(0,matchIndex),temp});
                    temp = temp->children.back().second;   
                    temp->filePos.push_back({pdir, offset});
                    return;
                }
            }  
        }

        //insertar lo que queda de la palabra en el padre
        if(notFound) {
            if(DEBUG_MODE) cout<<"7\n";
            cur->children.push_back({word.substr(index),new Node(true)});
            cur = (*cur)[word.substr(index)].second;
            cur->filePos.push_back({pdir, offset});
            cur->state = true;
            return;
        }
    }

}

void Patricia::deleteWord(string word){
    Node* cur = root;
    int index = 0;
    stack<Node*> route;
    while(index < word.size()){
        if(cur->exists(string(1,word[index]))){//key found
            route.push(cur);
            cur = (*cur)[string(1,word[index])].second;
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
            cur->erase(string(1,word[index]));
            index--;
        }
        if(cur->children.size() != 0){
            return;
        }
    }
    
}

size_t Patricia::count(Node* x) {
    int cnt = x->filePos.size();
    for (auto c : x->children) {
        cnt += count(c.second);
    }
    return cnt;
}

vector<string> Patricia::getPaths(string word) {
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

size_t Patricia::prefix(string prefix){
    Node* cur = root;
    int index = 0;
    while(index < prefix.size()){
        SEARCH_COUNTER++;
        for(auto c : cur->children) {
            // c.first = transicion de padre a hijo
            // index = curr pos del prefijo
            if(c.first[0] == prefix[index]) {
                if(index + c.first.size() <= prefix.size() && c.first == prefix.substr(index,c.first.size())) {
                    index += c.first.size();
                    cur = c.second; // me voy al hijo
                    break;
                } 
                else if (c.first.substr(0,prefix.length()-index) == prefix.substr(index)) {
                    index = prefix.length();
                    cur = c.second;
                    return count(cur);
                }
            }
            if(c.first == cur->children.back().first) { // ninguna transicion encaja
                return 0;
            }
        }
    }
    return count(cur); // prefijo es palabra
}

vector< pair<unsigned int, unsigned int> > Patricia::search(string word){
    Node* cur = root;
    int index = 0;
    while(index < word.size()){
        SEARCH_COUNTER++;
        for(auto c: cur->children) {
            if(c.first[0] == word[index]) {
                if(index + c.first.size() <= word.size() && c.first == word.substr(index,c.first.size())) {
                    index += c.first.size();
                    cur = c.second;
                    break;
                }
                return vector<pair<unsigned int, unsigned int>>();
            }
            if(c.first == cur->children.back().first) {
                return vector<pair<unsigned int, unsigned int>>();
            }
        }
    }
    return cur->filePos;
}

void Patricia::print(){
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

void Patricia::print(Node* node, int l){
    for(const auto pair: node->children){
        for(int i = 0; i < l; i++) cout<<"\t";
        cout<<pair.first<<"\n";
        print((*node)[pair.first].second, l+1);
    }
}

void Patricia::build(string filename){ 
    if(DEBUG_MODE) cout<<"buildbegin\n";
    ifstream file(filename);
    ofstream keys("keys.db");
    string line, key;
    unsigned int pgdir = 0, offset;
    while(getline(file,line)) {
        offset = (unsigned int)file.tellg() - pgdir;
        if(DEBUG_MODE) cout<<"=================================================================\n";
        key = getFileNameFromRoute(line);
        if(DEBUG_MODE) cout<<"Inserting: "<<key<<"\n";
        keys<<key<<"\n";
        insert(key, pgdir, offset);
        if(DEBUG_MODE) cout<<"=================================================================\n";
        pgdir = file.tellg();
    }
    if(DEBUG_MODE) cout<<"buildend\n";
    file.close();
}

void Patricia::pNode(Node* p) {
    for(auto e: p->children) {
        cout<<e.first<<"\n";
    }
}
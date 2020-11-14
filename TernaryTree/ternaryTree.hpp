#ifndef TERNARY_TREE_H
#define TERNARY_TREE_H

#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <unordered_map>

unsigned reads = 0;

using namespace std;

struct Node{
    bool isTerminal = false; // Indica que el anterior es terminal
    char letra;
    Node* center_child = nullptr;
    Node* right_child = nullptr;
    Node* left_child = nullptr;
    Node* parent = nullptr;
    vector<pair<unsigned, unsigned>> filePos;

    Node(){};

    Node(char value) {
        letra = value;
    }

    bool hijosValidos() {
        return center_child || right_child || left_child;
    }
};

class TernaryTree{
    Node* root = nullptr;

    void completar (string value, Node* nodo, unsigned pos, unsigned offset) {
        size_t i = 0;
        while (i < value.length()) {
            Node* hijo = new Node(value[i++]);
            nodo->center_child = hijo;
            hijo->parent = nodo;
            nodo = nodo->center_child;
        }
        Node* nil = new Node();
        nil->isTerminal = true;
        nodo->center_child = nil;
        nil->parent = nodo;

        nodo->filePos.push_back({pos, offset});
    }

public:
    void insert(string value, unsigned pos = 0, unsigned offset = 0) {
        if (root) {
            Node* c_root = root;
            size_t i = 0;
            while (i < value.length()) {
                if (value[i] == c_root->letra) {
                    if (c_root->center_child->isTerminal) {
                        if (i < value.length() - 1) {
                            c_root->center_child->letra = value[++i];
                            completar(value.substr(i + 1), c_root->center_child, pos, offset);
                            return;
                        }
                    }
                    c_root = c_root->center_child;
                    ++i;
                    if (i == value.length() - 1) {
                        c_root->filePos.push_back( {pos, offset} );
                        c_root->center_child->isTerminal = true;
                    }
                } else if (value[i] > c_root->letra) {
                    if (!c_root->right_child) {
                        Node* tmp = new Node(value[i++]);
                        c_root->right_child = tmp;
                        completar(value.substr(i), tmp, pos, offset);
                        return;
                    }
                    c_root = c_root->right_child;
                } else {
                    if (!c_root->left_child) {
                        Node* tmp = new Node(value[i++]);
                        c_root->left_child = tmp;
                        completar(value.substr(i), tmp, pos, offset);
                        return;
                    }
                    c_root = c_root->left_child;
                }
            }
        } else {
            root = new Node(value[0]);
            Node* c_root = root;
            size_t i = 1;
            while (i < value.length()) {
                Node* hijo = new Node(value[i++]);
                c_root->center_child = hijo;
                hijo->parent = c_root;
                c_root = c_root->center_child;
            }
            Node* nil = new Node();
            nil->isTerminal = true;
            c_root->center_child = nil;
            nil->parent = c_root;
            c_root->filePos.push_back( {pos, offset} );
        }
    }

    Node* getRoot() {
        return root;
    }

    vector<pair<unsigned, unsigned>> search(string value) {
        Node* c_root = root;
        size_t i = 0;
        while ( i < value.length() && c_root->hijosValidos() ) {
            ++reads;
            if (value[i] == c_root->letra) {
                if (!c_root->center_child)
                    return {};
                c_root = c_root->center_child;
                ++i;
            } else if (value[i] > c_root->letra) {
                if (!c_root->right_child)
                    return {};
                c_root = c_root->right_child;
            } else {
                if (!c_root->left_child)
                    return {};
                c_root = c_root->left_child;
            }
        }
        
        if (c_root->isTerminal) {
            return c_root->parent->filePos;
        }
        else
            return {};
    }

    vector<string> build(string file) {
        unordered_map<string, bool> hash;
        ifstream myfile (file, ios::binary);
        vector<string> keys;
        string key, line;
        unsigned int pgdir = 0, offset;

        while(getline(myfile, line)) {
            offset = (unsigned int) myfile.tellg() - pgdir;

            key = getFileNameFromRoute(line);
            if (!hash[key]) { keys.push_back(key); }
            hash[key] = true;
            insert(key, pgdir, offset);

            pgdir = myfile.tellg();
        }

        myfile.close();
        return keys;
    }

    void prefix(string text) {
        auto pre = searchPrefix(text);
        if (pre) {
            prefixes(pre, text);
        }
    }

private:
    void prefixes(Node* nodo, string text) {
        if (nodo->left_child) {
            string tmp = text;
            tmp[tmp.length() - 1] = nodo->left_child->letra; 
            prefixes(nodo->left_child, tmp);
        }
        if (nodo->center_child) {
            if (nodo->center_child->isTerminal) {
                cout << text << '\n';
            }
            string tmp = text + nodo->center_child->letra;
            prefixes(nodo->center_child, tmp);
        }
        if (nodo->right_child) {
            string tmp = text;
            tmp[tmp.length() - 1] = nodo->right_child->letra; 
            prefixes(nodo->right_child, tmp);
        }
    }

    Node* searchPrefix(string value) {
        Node* c_root = root;
        size_t i = 0;
        while ( i < value.length() && c_root->hijosValidos() ) {
            ++reads;
            if (value[i] == c_root->letra) {
                if (!c_root->center_child)
                    return {};
                c_root = c_root->center_child;
                ++i;
            } else if (value[i] > c_root->letra) {
                if (!c_root->right_child)
                    return {};
                c_root = c_root->right_child;
            } else {
                if (!c_root->left_child)
                    return {};
                c_root = c_root->left_child;
            }
        }
        
        return c_root->parent;
    }

    string getFileNameFromRoute(string s) {
        string delimiter = "/";

        size_t pos = 0;
        string token;
        while ((pos = s.find(delimiter)) != string::npos) {
            token = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
        }

        if ((pos = s.find_last_of(".pdf")) != string::npos) {
            token = s.substr(0, pos - 3);
            return token;
        }
        return "error";
    }
};

#endif
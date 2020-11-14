#include <vector>

using namespace std;
class Trie;

class Node{
private:
    friend class Trie;
    bool state;
    vector<pair<char,Node*>> children;
    //Start, offset
    vector<pair<unsigned int,unsigned int>> filePos;
public:
    Node();
    Node(bool _state);
    ~Node();

    bool exists(char key){
        for(auto e: children){
            if(e.first == key){
                return true;
            }
        }
        return false;
    }

    void erase(char key){//cambiar
        for(auto it = children.begin(); it != children.end();it++){
            if(it->first == key){
                children.erase(it);
                return;
            }
        }
    }

    pair<char,Node*> operator [](char key){
        for(auto e: children){
            if(e.first == key){
                return e;
            }
        }
    }
};

Node::Node(){
    this->state = false;
}

Node::Node(bool _state){
    this->state = _state;
}

Node::~Node(){
}

#include <vector>

using namespace std;
class Patricia;

class Node{
private:
    friend class Patricia;
    bool state;
    vector< pair<string,Node*> > children;
    //Start, offset
    vector<pair<unsigned int,unsigned int>> filePos;
public:
    Node();
    Node(bool _state);
    ~Node();

    bool exists(string key){
        for(auto e: children){
            if(e.first == key){
                return true;
            }
        }
        return false;
    }

    void erase(string key){//cambiar
        for(auto it = children.begin(); it != children.end();it++){
            if(it->first == key){
                children.erase(it);
                return;
            }
        }
    }

    pair<string,Node*> operator [](string key){
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

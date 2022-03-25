// cell.h
#ifndef CELL_H
#define CELL_H

#include <string>
#include <vector>
using namespace std;

class Net;
class Node;

class Cell {
public:
    // constructor and destructor
	Cell(const string name, const int cell_id) {
        gain_ = 0;
        lock_ = 0;
        part_ = 0;
        node_ = new Node(this);
        name_ = name;
    }
	~Cell(){
        delete node_;
    }

    // get and set
    int GetGain()    const { return gain_; }
    bool IsLock()    const { return lock_; }
    bool GetPart()   const { return part_; }
    Node* GetNode()  const { return node_; }
    string GetName() const { return name_; }
    int GetNumPins() const { return net_list_.size(); }
    Net* GetNet(const int i) const { return net_list_[i]; }
    void SetGain(const int gain)    { gain_ = gain; }
    void SetPart(const bool part)   { part_ = part; }

    // modify
    void IncGain() { ++gain_; }
    void DecGain() { --gain_; }
    void Lock()    { lock_ = true; }
    void Unlock()  { lock_ = false; }
    void Move()    { part_ = 1 - part_; }
    void AddNet(Net* net) { net_list_.push_back(net); }

private:
    int gain_;
    bool lock_;
    bool part_;
    Node* node_; 
	string name_;
    vector<Net*> net_list_;
};

#endif

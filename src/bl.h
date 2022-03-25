// bl.h
#ifndef BL_H
#define BL_H

#include <vector>
using namespace std;

#include "node.h"
#include <cassert>
#include <map>
#include <iostream>
using namespace std;

class BucketList{
public:
    // constructor and destructor
    BucketList(int max_num_pins) {
        max_gain_ = -max_num_pins;
        max_num_pins_ = max_num_pins;
        for (int i = -max_num_pins_; i <= max_num_pins_; ++i) {
            Node* head_node = new Node(nullptr);
            head_nodes_.insert( pair<int, Node*>(i, head_node));
        }
    }
    ~BucketList() {
        for (int i = -max_num_pins_; i <= max_num_pins_; ++i)
            delete head_nodes_[i];
    }
          
    // get and set
    int GetMaxGain() const { return max_gain_; }
    Node* GetMaxGainNode() {
        assert(head_nodes_.find(max_gain_) != head_nodes_.end());
        return head_nodes_[max_gain_]->GetNext();
    }
    void Reset() {
        for (int i = -max_num_pins_; i <= max_num_pins_; ++i) {
            head_nodes_[i]->SetNext(nullptr);
        }
    }

    // modify
    void DeleteMaxGainNode() { // (head)-(node)-(next) >> head-next
        Node* head = head_nodes_[max_gain_];
        Node* node = head->GetNext();
        assert(node != nullptr);
        Node* next = node->GetNext();       
        
        head->SetNext(next);
        node->SetPrev(nullptr);
        node->SetNext(nullptr);
        if (next != nullptr)
            next->SetPrev(head);
        // find next max gain
        while( max_gain_ > -max_num_pins_ && head_nodes_[max_gain_]->GetNext() == nullptr) {
            max_gain_--;
        }
    }
    void DeleteNode(Node* node) { // (prev)-(node)-(next) >> (prev)-(next)
        Node* prev = node->GetPrev();
        Node* next = node->GetNext();

        if (prev != nullptr)
            prev->SetNext(next);
        node->SetPrev(nullptr);
        node->SetNext(nullptr);
        if (next != nullptr)
            next->SetPrev(prev);
    }
    void InsertNode(Node* node, int gain) { // head-next >> head-node-next
        assert(head_nodes_.find(gain) != head_nodes_.end());
        Node* next = head_nodes_[gain]->GetNext();
        
        head_nodes_[gain]->SetNext(node);
        node->SetPrev(head_nodes_[gain]);
        node->SetNext(next);
        if (next != nullptr)
            next->SetPrev(node);
        
        if (gain > max_gain_)
            max_gain_ = gain;
    }
   
private:
    int max_gain_;
    int max_num_pins_;
    map<int, Node*> head_nodes_;
};

#endif

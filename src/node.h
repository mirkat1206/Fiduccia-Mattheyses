// node.h
#ifndef NODE_H
#define NODE_H

class Cell;

class Node {
public:
    // constructor and destructor
    Node(Cell* cell) {
        cell_ = cell;
        prev_ = nullptr;
        next_ = nullptr;
    }
    ~Node(){}

    // get and set
    Cell* GetCell() const { return cell_; }
    Node* GetPrev() const { return prev_; }
    Node* GetNext() const { return next_; }
    void SetPrev(Node* prev) { prev_ = prev; }
    void SetNext(Node* next) { next_ = next; }
    
private:
    Cell* cell_;
    Node* prev_;
    Node* next_;
};

#endif

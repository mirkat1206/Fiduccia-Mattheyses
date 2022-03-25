// net.h
#ifndef NET_H
#define NET_H

#include <string>
#include <vector>
using namespace std;

class Cell;

class Net {
public:
    // constructor and destructor
	Net(string name) {
        part_size_[0] = 0;
        part_size_[1] = 0;
        name_ = name;
    }
	~Net(){}

    // get and set
    string GetName()                const { return name_; }
    int GetPartSize(const int part) const { return part_size_[part]; }
    int GetCellListSize()           const { return cell_list_.size(); }
    Cell* GetCell(const int i)      const { return cell_list_[i]; }
    void SetPartSize(const int part, const int gain) { part_size_[part] = gain; }
    
    // modify
    void IncPartSize(const int part)   { ++part_size_[part]; }
    void DecPartSize(const int part)   { --part_size_[part]; }
    void AddCell(Cell* cell) { cell_list_.push_back(cell); }

private:
    int part_size_[2];
	string name_;
    vector<Cell*> cell_list_;
};

#endif

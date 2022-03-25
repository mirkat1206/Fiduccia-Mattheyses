// fm.h
#ifndef FM_H
#define FM_H

//#define DEBUG

#ifdef DEBUG
#define DEBUG_MSG(str) do { cout << str << endl; } while (false)
#else
#define DEBUG_MSG(str) do { } while (false)
#endif

#include "bl.h"
#include "cell.h"
#include "net.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

class FiducciaMattheyses {
public:
    // constructor and destructor
    FiducciaMattheyses(ifstream &fin) {
        num_cells_    = 0;
        num_nets_     = 0;
        max_num_pins  = 0;
        part_size_[0] = 0;
        part_size_[1] = 0;
        ParseInput(fin);
    }
    ~FiducciaMattheyses() {
        int size;
        size = cell_list_.size();
        for (int i = 0; i < size; ++i)
            delete cell_list_[i];
        size = net_list_.size();
        for (int i = 0; i < net_list_.size(); ++i)
            delete net_list_[i];
        delete buckets_[0];
        delete buckets_[1];
    }

    // get and set
    int GetNumCells() const { return num_cells_; }
    int GetNumNets()  const { return num_nets_; }
    int GetPartSize(const int part) const { return part_size_[part]; }
    
    // input and output
    void ParseInput(ifstream &fin);
    void PrintSummary();
    void ReportCells();
    void ReportNets();
    void WriteResult(ofstream &fout);

    // solver
    void Solve();
    void InitPartiotion();
    void UnlockAllCells();
    void InitNetPartSize();
    void InitCellGain();
    Cell* FindMaxGainCell();
    bool IsMoveLegal(Cell* move_cell);
    void MoveCell(Cell* move_cell);
    void UpdateCellGain(Cell* move_cell);
    void UndoMoves(int max_index);

    // others
    int CalculateCutSize();

private:
    // basic
    int num_cells_;
    int num_nets_;
    int max_num_pins;
    int part_size_[2];
    double balance_;
    double lower_bound_;
    double upper_bound_;
    map<string, int> cell_name2id_;
    map<string, int> net_name2id_;
    vector<Cell*> cell_list_;
    vector<Net*> net_list_;

    // fm algorithm
    vector<int> gain_stack_;
    vector<Cell*> move_stack_;
    BucketList* buckets_[2];
};

#endif

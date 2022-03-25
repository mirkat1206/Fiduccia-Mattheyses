// fm.cpp
#include "fm.h"

/* ---------- input and output---------- */
void FiducciaMattheyses::ParseInput(ifstream &fin) {
    // <Balance Degree>
    fin >> balance_; 
    
    // NET <Net Name> [<Cell Name>]+ ;
    int net_id, cell_id;
    string net_name, cell_name;            
    while (fin >> net_name) {
        if (net_name == "NET") {
            fin >> net_name;
            net_id = num_nets_++;
            Net* new_net = new Net(net_name);
            net_name2id_[net_name] = net_id;
            net_list_.push_back(new_net);

            while (fin >> cell_name) {
                if (cell_name == ";")
                    break;
                Cell* new_cell;
                // a newly seen cell
                if (cell_name2id_.find(cell_name) == cell_name2id_.end()) {
                    cell_id = num_cells_++;
                    new_cell = new Cell(cell_name, cell_id);
                    cell_name2id_[cell_name] = cell_id;
                    cell_list_.push_back(new_cell);                    
                }
                // an existed cell
                else {
                    cell_id = cell_name2id_[cell_name];                    
                    new_cell = cell_list_[cell_id];
                }
                new_cell->AddNet(new_net);
                new_net->AddCell(new_cell);                
            }
        } else {
            cerr << "Input file format error." << endl
                 << "Correct format: NET <Net Name> [<Cell Name>] ;" << endl;
            exit(-1);
        }
    }    
    
    for (int i = 0; i < num_cells_; ++i) {
        int temp = cell_list_[i]->GetNumPins();
        if (max_num_pins < temp)
            max_num_pins = temp;
    }    

    lower_bound_ = (1 - balance_) * num_cells_ / 2.0;
    upper_bound_ = (1 + balance_) * num_cells_ / 2.0;

    buckets_[0] = new BucketList(max_num_pins);
    buckets_[1] = new BucketList(max_num_pins);
}

void FiducciaMattheyses::PrintSummary() {
    cout << endl
         << "==================== Summary ====================" << endl
         << "Cutsize: " << this->CalculateCutSize() << endl
         << "Total cell number: " << num_cells_ << endl
         << "Total net number: " << num_nets_ << endl
         << "Lower bound number: " << lower_bound_ << endl
         << "Upper bound number: " << upper_bound_ << endl
         << "Cell number of partition A: " << part_size_[0] << endl
         << "Cell number of partition B: " << part_size_[1] << endl
         << "=================================================" << endl
         << endl;         
}

void FiducciaMattheyses::ReportCells() {
    cout << "Number of cells: " << num_cells_ << endl;
    for (int i = 0; i < num_cells_; ++i) {
        cout << cell_list_[i]->GetName() << "(" << cell_list_[i]->GetPart() << "): \t";
        int size = cell_list_[i]->GetNumPins();
        for (int j = 0; j < size; ++j) {
            Net* net = cell_list_[i]->GetNet(j);
            cout << net->GetName() 
                 << "(" << ((net->GetPartSize(0) && net->GetPartSize(1)) ? "cut" : "uncut") << ") ";
        }
        cout << endl;
    }
}

void FiducciaMattheyses::ReportNets() {
}

void FiducciaMattheyses::WriteResult(ofstream &fout) {
    int cnt; 
    string s;

    fout << "Cutsize = " << this->CalculateCutSize() << endl;
    
    cnt = 0;
    s = "";
    for (int i = 0; i < num_cells_; ++i) {
        if (cell_list_[i]->GetPart() == 0) {
            cnt++;
            s += cell_list_[i]->GetName();
            s += " ";
        }
    }
    fout << "G1 " << cnt << "\n" << s << ";" << endl;
    
    cnt = 0;
    s = "";
    for (int i = 0; i < num_cells_; ++i) {
        if (cell_list_[i]->GetPart() == 1) {
            cnt++;
            s += cell_list_[i]->GetName();
            s += " ";
        }
    }
    fout << "G2 " << cnt << "\n" << s << ";" << endl;
}

/* ---------- solver ---------- */
void FiducciaMattheyses::Solve() {
    DEBUG_MSG("Start Solving...");

    this->InitPartiotion();
    this->InitNetPartSize();
    DEBUG_MSG("InitPartiotion finished...");

    int iter = 0;
    int index_constrain = -1;
    int n = 10;
    do {
        this->UnlockAllCells();
        this->InitCellGain();
        DEBUG_MSG("UnlockAllCells, InitCellGain finished...");
        
        cout << "------------------------------------------------------------------------" << endl
             << "Iternation " << iter++ << " :\tcut_size_ = " << this->CalculateCutSize();
        DEBUG_MSG("");

        gain_stack_.clear();
        move_stack_.clear();
        vector<Cell*> temp;
        int move_cnt = 0, total_gain = 0, max_gain = 0, max_index = 0;
        bool bug_flag = false;
        do {
            Cell* move_cell = this->FindMaxGainCell();            
            if (move_cell == nullptr)
                break;
            //DEBUG_MSG("FindMaxGainCell finished...");
            
            bool from = move_cell->GetPart();
            buckets_[from]->DeleteMaxGainNode();        

            //assert(move_cell->IsLock() == false);
            /*
             * Bugs here.
             * Assertion should not happan...
             * Should not have to check IsLock()...
             */
            if (move_cell->IsLock()) {
                bug_flag = true;
                break;
            }
            else if (this->IsMoveLegal(move_cell)) {                
                move_cnt++;
                gain_stack_.push_back(move_cell->GetGain());
                move_stack_.push_back(move_cell);

                // three functions cannot swap the order!!
                move_cell->Lock();
                this->UpdateCellGain(move_cell);
                //DEBUG_MSG("UpdateCellGain finished...");
                this->MoveCell(move_cell);
                //DEBUG_MSG("MoveCell finished...");

                total_gain += move_cell->GetGain();
                if (total_gain > max_gain) {
                    max_gain = total_gain;
                    max_index = move_cnt;
                }
                
                for (int i = 0; i < temp.size(); ++i)
                    buckets_[temp[i]->GetPart()]->InsertNode(temp[i]->GetNode(), temp[i]->GetGain());
                temp.clear();
            } else {
                temp.push_back(move_cell);
            }
            //DEBUG_MSG(move_cell->GetName() << ": gain = " << move_cell->GetGain() << "\t cut_size = " << this->CalculateCutSize());
        } while(index_constrain == -1 || move_cnt < index_constrain);
        //} while(index_constrain == -1 || move_cnt < num_cells_);
        
        DEBUG_MSG("while finished...");
        
        if (index_constrain == -1 && bug_flag == false)
            index_constrain = max_index;

        cout << "\tmax_gain = " << max_gain << "\tmax_index = " << max_index << endl << endl;
        
        this->UndoMoves(max_index);
        DEBUG_MSG("UnlockAllCells finished...");
 
        if (max_gain <= 0)
            break;
    } while(1);
    cout << "------------------------------------------------------------------------" << endl;
    DEBUG_MSG("FM finished...");
}

void FiducciaMattheyses::InitPartiotion() {
    Cell* cell;
    // put in part[0]
    for (int i = 0; i < num_cells_ / 2; ++i) {
        cell = cell_list_[i];
        cell->SetPart(0); 
        part_size_[0]++;
    }
    // put in part[1]
    for (int i = num_cells_ / 2; i < num_cells_; ++i) {
        cell = cell_list_[i];
        cell->SetPart(1); 
        part_size_[1]++;
    }
    
    Net* net;
    for (int i = 0; i < num_nets_; ++i) {
        net = net_list_[i];
        net->SetPartSize(0, 0);
        net->SetPartSize(1, 0);
        int size = net->GetCellListSize();
        for (int j = 0; j < size; ++j) 
            net->IncPartSize(net->GetCell(j)->GetPart());
    }
}

void FiducciaMattheyses::UnlockAllCells() {
    for (int i = 0; i < num_cells_; ++i)
        cell_list_[i]->Unlock();
}

void FiducciaMattheyses::InitNetPartSize() {
    Cell* cell;
    Net* net;
    for (int i = 0; i < num_nets_; ++i) {
        net = net_list_[i];
        net->SetPartSize(0, 0);
        net->SetPartSize(1, 0);
        int size = net->GetCellListSize();
        for (int j = 0; j  < size; ++j) {
            cell = net->GetCell(j);
            net->IncPartSize(cell->GetPart());
        }
    }
}

void FiducciaMattheyses::InitCellGain() {
    buckets_[0]->Reset();
    buckets_[1]->Reset();
    
    Cell* cell;
    Net* net;
    for (int i = 0; i < num_cells_; ++i) {
        cell = cell_list_[i];
        cell->SetGain(0);
        bool from = cell->GetPart(), to = 1 - from;
        int size = cell->GetNumPins();
        for (int j = 0; j < size; ++j) {
            net = cell->GetNet(j);
            if (net->GetPartSize(from) == 1)
                cell->IncGain();
            if (net->GetPartSize(to) == 0)
                cell->DecGain();
        }
        buckets_[from]->InsertNode(cell->GetNode(), cell->GetGain());
    }
}

Cell* FiducciaMattheyses::FindMaxGainCell() {
    Node* max_gain_node;
    if (part_size_[0] == lower_bound_)
        max_gain_node = buckets_[1]->GetMaxGainNode();
    else if (part_size_[1] == lower_bound_)
        max_gain_node = buckets_[0]->GetMaxGainNode();
    else if (buckets_[0]->GetMaxGain() > buckets_[1]->GetMaxGain())        
        max_gain_node = buckets_[0]->GetMaxGainNode();
    else
        max_gain_node = buckets_[1]->GetMaxGainNode();
    
    if (max_gain_node == nullptr)
        return nullptr;
    return max_gain_node->GetCell();
}

bool FiducciaMattheyses::IsMoveLegal(Cell* move_cell) {
    bool from = move_cell->GetPart(), to = 1 - from;
    return ( (part_size_[from] - 1) >= lower_bound_ &&
             (part_size_[to] + 1) <= upper_bound_ );
}

void FiducciaMattheyses::MoveCell(Cell* move_cell) {
    bool from = move_cell->GetPart(), to = 1 - from;

    // cell    
    move_cell->Move();

    // net
    Net* net;
    int size = move_cell->GetNumPins();
    for (int i = 0; i < size; ++i) {
        net = move_cell->GetNet(i);
        net->DecPartSize(from);
        net->IncPartSize(to);
    }

    // fm
    part_size_[from]--;
    part_size_[to]++;
}

void FiducciaMattheyses::UpdateCellGain(Cell* move_cell) {
    bool from = move_cell->GetPart(), to = 1 - from;

    Cell* cell;
    Net* net;    
    int size = move_cell->GetNumPins();
    for (int i = 0; i < size; ++i) {
        net = move_cell->GetNet(i);
        int F = net->GetPartSize(from), T = net->GetPartSize(to);
        if (T == 0) {
            for (int j = 0; j < net->GetCellListSize(); ++j) {
                cell = net->GetCell(j);
                if (cell->IsLock() == false) {
                    cell->IncGain();
                    buckets_[from]->DeleteNode(cell->GetNode());
                    buckets_[from]->InsertNode(cell->GetNode(), cell->GetGain());
                }
            }
        } else if (T == 1) {
            for (int j = 0; j < net->GetCellListSize(); ++j) {
                cell = net->GetCell(j);
                if (cell->IsLock() == false && cell->GetPart() == to) {                    
                    cell->DecGain();
                    buckets_[to]->DeleteNode(cell->GetNode());
                    buckets_[to]->InsertNode(cell->GetNode(), cell->GetGain());
                    break;
                }
            }
        }

        F--;
        T++;

        if (F == 0) {            
            for (int j = 0; j < net->GetCellListSize(); ++j) {
                cell = net->GetCell(j);
                if (cell->IsLock() == false) {                    
                    cell->DecGain(); 
                    buckets_[to]->DeleteNode(cell->GetNode());
                    buckets_[to]->InsertNode(cell->GetNode(), cell->GetGain());
                }
            }

        } else if (F == 1) {
            for (int j = 0; j < net->GetCellListSize(); ++j) {
                cell = net->GetCell(j);
                if (cell->IsLock() == false && cell->GetPart() == from) {                    
                    cell->IncGain();
                    buckets_[from]->DeleteNode(cell->GetNode());
                    buckets_[from]->InsertNode(cell->GetNode(), cell->GetGain());
                    break;
                }
            }
        }
    }
}

void FiducciaMattheyses::UndoMoves(int max_index) {
    int size = move_stack_.size();
    for (int i = max_index; i < size; ++i) {
        this->MoveCell(move_stack_[i]);
    }
}

/* ---------- others ---------- */
int FiducciaMattheyses::CalculateCutSize() {
    this->InitNetPartSize();
    int cut_size_ = 0;
    Net* net;
    for (int i = 0; i < num_nets_; ++i) {
        net = net_list_[i];
        if (net->GetPartSize(0) && net->GetPartSize(1))
            cut_size_++;
    }
    return cut_size_;
}

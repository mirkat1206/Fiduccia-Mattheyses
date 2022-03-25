// main.cpp
#include "fm.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[])
{
    ifstream fin;
    ofstream fout;
    if (argc == 3) {
        fin.open(argv[1]);
        if (!fin) {
            cerr << " Cannot open the input file \"" << argv[1] << "\"." << endl
                 << " The program will be terminated..." << endl;
            exit(1);
        }
        fout.open(argv[2]);
        if (!fout) {
            cerr << " Cannot open the input file \"" << argv[2] << "\"." << endl
                 << " The program will be terminated..." << endl;
            exit(1);
        }
    } else {
        cerr << "Usage: ./fm <input file> <output file>" << endl;
        exit(1);
    }
    FiducciaMattheyses fm(fin);
    
    clock_t start, end;
    start = clock();
    
    fm.Solve();
    
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    cout << "Time taken by fm is : " << fixed << setprecision(5)
         << time_taken << " secends." << endl;

    fm.PrintSummary();
    fm.WriteResult(fout);       

    return 0;
}

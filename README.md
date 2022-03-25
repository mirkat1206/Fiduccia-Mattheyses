# Fiduccia-Mattheyses Heuristic
Programming Asignment #1 of Physical Design for Nanometer ICs, Spring 2022

|系級|學號|姓名|日期|
|:---:|:---:|:---:|:---:|
|LOL|UWU|OUO|2022-3-26|

## How to compile?

To generate an executable binary `./bin/fm`, simply type `make` in the current directory.

```
make
```

## How to execute?

Use the following command format to execute in the current directory.

```
./bin/fm <input file name> <output file name>
```

## Program Structure
```bash
├─ src
    ├─ main.cpp  // Main
    ├─ cell.h    // Cell header file
    ├─ net.h     // Net header file
    ├─ bl.h      // Bucket List header file
    ├─ fm.h      // Fiduccia-Mattheyses header file
    ├─ fm.cpp    // Fiduccia-Mattheyses 
```

## Features
- Implementation of Fiduccia-Mattheyses Heuristic.
- Initail partition
    - Simply divide the cells into 2 halves in the middle.
- Timing optimiaztion
    - In the original Fiduccia-Mattheyses Heuristic, for each iteration it will **move all cells** and find the maximum partial gain.
    - Based on my observation, the number of **real moves**, which generate the maximum partial gain, is maximum at the first iteration.
    - Therefore, rather than trying to move all cells and find the maximum partial gain, I set the the number of real moves of the first iteration (suppose it is `index_constrain`) as the constraint and only try to find maximum partial sum within `index_constrain` moves.
- Following [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

## Issues
- Bugs in `./src/fm.cpp` line 153.
    - The assertion should not happen.
    - Handle the bugs by `if` in line 159 for now.
- The max gain calculated and the real gain sometimes does not match (ex: input_0.dat).
- `Class BucketList` can be merged into `class FiducciaMattheyses`.
- Initial partition can be optimized.

## 2-Way F-M Circuit Partitioning
### Problem Description
- Given `k` cells, `m` nets, balance degree `d`, partition the cells into two disjoint, balanced groups $G1$ and $G2$.
- Balance constraint: $\frac{1 - d}{2} \times k \leq \#(G1), \#(G2) \leq \frac{1 + d}{2} \times k$


### Input File Format
```
<Balance Degree>
NET <Net Name> [<Cell Name>]+ ;
```

Example:

```
0.9
NET n1 c1 c2 c3 c4 ;
NET n2 c1 c5 ;
NET n3 c2 c5 ;
NET n4 c3 c6 ;
NET n5 c4 c6 ;
NET n6 c4 c6 ;
```

### Output File Format
```
Cutsize = <Number>
G1 <Size>
[<Cell Name>]+ ;
G2 <Size>
[<Cell Name>]+ ;
```

Example:

```
Cutsize = 1
G1 3
c1 c2 c5 ;
G2 3
c3 c4 c6 ;
```

## Roadmap
- [x] Understand F-M Partitioning Heuristic
- [x] Define data structures
    - [x] node.h
    - [x] cell.h
    - [x] net.h
    - [x] bl.h
    - [x] fm.h
- [x] Implement F-M Partitioning Heuristic
    - [x] Parse input files
    - [x] Print output format
    - [x] Data Structure
        - [x] Cell
        - [x] Net
        - [x] BucketList
    - [x] Solve()
        - [x] InitPartition()
        - [x] UnlockAllCells()
        - [x] InitNetPartSize()
        - [x] InitCellGain()
        - [x] FindMaxGainCell()
        - [x] IsMoveLegal()
        - [x] MoveCell()
        - [x] UpdateCellGain()
        - [x] UndoMoves()
    - [x] Timing control

### Experiment
The `Execution Time` refers to the original Fiduccia-Mattheyses Heuristic; the `Execution Time(opt.)` refers to my futher timing optimization version.

Note that my further optimization does not hurt the performance at all.

|Test Case|# of Cells|# of Nets|Initial Cutsize|Final Cutsize|Execution Time|Execution Time(opt.)|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|input_0|150750|166998|65799|12742|2.02 sec|0.78 sec|
|input_1|3000|5000|2400|1692|0.00 sec|0.00 sec|
|input_2|7000|10000|4470|2198|0.06 sec|0.04 sec|
|input_3|66666|88888|47238|27845|7.19 sec|7.12 sec|
|input_4|150750|166998|82801|45045|38.02 sec|26.03 sec|
|input_5|382489|483599|251653|143932|210.19 sec|171.71 sec|

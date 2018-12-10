# Sokoban-Game

The program should be compiled and run in Linux compatible environment.

To compile, run:
g++ -std=c++14 sokoban.cpp
in terminal, in the same directory of where sokoban.cpp is placed. You must have GCC installed with version 5.2 or higher.

To run the program with benchmarks:
./a.out [test number] [algorithm number]

- Test number: this argument specifies which test to run in the inputs directory. To run test sokoban00.txt, use 00 for this argument.
- Algorithm number: 1 for BFS, 2 for DFS, 3 for greedy best-first search.

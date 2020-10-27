#include "source/Position.h"
#include "source/ZobristHash.h"
#include "source/Move.h"
#include "source/PieceTable.h"
#include "source/Board.h"
#include <iostream>

int main()
{
    int i;
    Position start({1, 0, -1});
    std::vector<std::vector<int>> start_neighbors = start.getAllNeighbors();
    std::vector<std::vector<int>>::iterator neighbor_iter;

    // for (neighbor_iter = start_neighbors.begin(); neighbor_iter < start_neighbors.end(); neighbor_iter++)
    // {
    //     for (i = 0; i < 3; i++)
    //     {
    //         std::cout << (*neighbor_iter)[i] << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    ZobristHash zHash;

    PieceTable pTable;

    //testing movement
    Move moveA("wQ");
    pTable.update(moveA);
    Move moveB("bQ", "wQ", 3, true);
    pTable.update(moveB);
    Move moveC("bQ", "wQ", 4);
    pTable.update(moveC);
    pTable.undoLast();
    pTable.undoLast();
    pTable.undoLast();

    std::cout << "Done" << std::endl;
};
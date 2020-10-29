#include "source/Position.h"
#include "source/ZobristHash.h"
#include "source/Move.h"
#include "source/PieceTable.h"
#include "source/Board.h"
#include "source/PieceInfo.h"
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

    std::map<int, int> defaultConfig
    {
        {PieceCodes::wQ, 1},
        {PieceCodes::wA, 3},
        {PieceCodes::wB, 2},
        {PieceCodes::wG, 3},
        {PieceCodes::wS, 2},
        {PieceCodes::bQ, 1},
        {PieceCodes::bA, 3},
        {PieceCodes::bB, 2},
        {PieceCodes::bG, 3},
        {PieceCodes::bS, 2}
    };

    Board board(defaultConfig);

    //testing movement
    Move moveA("wQ");
    board.makeMove(moveA);
    Move moveB("bQ", "wQ", 3, true);
    board.makeMove(moveB);
    Move moveC("bQ", "wQ", 4);
    std::vector<Move> moves = board.genAllMoves();
    board.makeMove(moveC);
    board.undoLast();
    board.undoLast();
    board.undoLast();

    std::cout << "Done" << std::endl;
};
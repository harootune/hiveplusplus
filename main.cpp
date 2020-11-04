#include "source/Position.h"
#include "source/ZobristHash.h"
#include "source/Move.h"
#include "source/PieceTable.h"
#include "source/Board.h"
#include "source/PieceInfo.h"
#include "source/Interface.h"
#include "source/Utils.h"
#include <iostream>

int main()
{
    // int i;
    // Position start({1, 0, -1});
    // std::vector<std::vector<int>> start_neighbors = start.getAllNeighbors();
    // std::vector<std::vector<int>>::iterator neighbor_iter;

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
    
    std::vector<Move> moves = board.genAllMoves();
    Move moveA("wA1");
    board.makeMove(moveA);
    moves = board.genAllMoves();
    Move moveB("bS1", "wA1", 3, true);
    board.makeMove(moveB);
    moves = board.genAllMoves();
    Move moveC("wG1", "wA1", 0, true); 
    board.makeMove(moveC);
    moves = board.genAllMoves();
    Move moveD("bB1", "bS1", 3, true);
    board.makeMove(moveD);
    moves = board.genAllMoves();
    Move moveE("wB1", "wA1", 1, true); 
    board.makeMove(moveE);
    moves = board.genAllMoves();
    Move moveF("bS2", "bS1", 4, true);
    board.makeMove(moveF);
    moves = board.genAllMoves();
    Move moveG("wQ", "wG1", 4, true);
    board.makeMove(moveG);
    moves = board.genAllMoves();
    Move moveH("bQ", "bB1", 2, true);
    board.makeMove(moveH);
    moves = board.genAllMoves();
    Move moveI("wQ", "wG1", 5, true);
    board.makeMove(moveI);
    moves = board.genAllMoves();

    int score = board.score();
    
    board.undoLast();
    board.undoLast();
    board.undoLast();

    bool check = Utils::isGameString("bQ1 \\wQ1");

    UHPInterface inter(defaultConfig);
    // inter.initTerminal();

    std::cout << "Done" << std::endl;
};
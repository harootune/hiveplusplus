#include "source/Position.h"
#include "source/ZobristHash.h"
#include "source/Move.h"
#include "source/Board.h"
#include "source/Engine.h"
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

    Engine engine(defaultConfig);
    std::vector<Move> moves;

    //testing movement
    
    // Move moveA("wA1");
    // Move moveB("bB1", "wA1", 1, true);
    // Move moveC("wQ", "wA1", 3, true);
    // Move moveD("bQ", "bB1", 0, true);

    Move moveA("wA1");
    Move moveB("bG1", "wA1", 5, true);
    Move moveC("wQ", "wA1", 3, true);
    Move moveD("bB1", "wQ", 0);
    // Move moveC("wB1", "wA1", 3, true);
    // Move moveD("bQ", "bB1", 0, true);
    // Move moveE("wQ", "wA1", 4, true);
    // Move moveF("bA1", "bB1", 1, true);
    // Move moveG("wB1", "bB1", 4);

    engine.makeMove(moveA);
    engine.makeMove(moveB);
    Move recc = engine.recommendMove();


    // board.makeMove(moveB);
    // board.makeMove(moveC);
    // board.makeMove(moveD);

    // board.makeMove(moveE);
    // board.makeMove(moveF);
    // board.makeMove(moveG);

    // board.undoLast();


    // Move recc = board.recommendMove();

    // moves = board.genAllMoves();

    

    // board.undoLast();
    // board.undoLast();
    // board.undoLast();

    // board.undoLast();
    // board.makeMove(moveA);

    

    // Move moveB("bS1", "wA1", 3, true);
    // board.makeMove(moveB);
    // board.undoLast();
    // board.undoLast();
    // board.makeMove(moveA);
    // board.makeMove(moveB);

    // Move moveC("wG1", "wA1", 0, true);
    // board.makeMove(moveC);
    // board.undoLast();
    // board.undoLast();
    // board.undoLast();
    // board.makeMove(moveA);
    // board.makeMove(moveB);
    // board.makeMove(moveC);

    // recc = board.recommendMove();


    bool check = Utils::isMoveString("bQ1 \\wQ1");

    UHPInterface inter(defaultConfig);
    // inter.initTerminal();

    std::cout << "Done" << std::endl;
};
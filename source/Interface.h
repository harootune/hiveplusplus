#pragma once

#ifndef _UHPINTERFACE_
#define _UHPINTERFACE_
#include <Engine.h>


namespace StandardPConfigs
{
    const std::map<int, int> base
    {
        {0, 1},
        {1, 3}, 
        {2, 2},
        {3, 3},
        {4, 2},
        {5, 1},
        {6, 3},
        {7, 2},
        {8, 3},
        {9, 2}
    };
};


class UHPInterface
// A UHP command line through which the Engine can be interacted with
{
    public:
        /* Constructors */
        UHPInterface();

        /* Misc */
        // Initialize the command line
        void initTerminal();
        
        /* Destructor */
        ~UHPInterface() {};
    
    private:
        /* Member Variables */
        // The engine which this UHPInterface connects to
        Engine _game;
        // Whether or not the current engine is active
        bool _active;
        // The engine's current mode (piececonfig)
        std::string _mode;
        
        /* Options Variables */
        // CustomPath
        std::string _customPath;
        // MaxTableSize - size of transposition table in MB
        int _tableSize;
        // DefaultDepth
        int _defaultDepth;

        /* Command Processing */
        // process an info command
        void _info(std::string input);
        // process a play command
        void _play(std::string input);
        // process a bestmove command
        void _bestMove(std::string input);
        // process an undo command
        void _undo(std::string input);
        // process a pass command
        void _pass(std::string input);
        // process a validmoves command
        void _validMoves(std::string input);
        // process a newgame command
        void _newGame(std::string input);
        // process an options command
        void _options(std::string input);
        // initialize a new engine
        bool _initGame(std::string input);
};

#endif
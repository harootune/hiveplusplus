#include "Interface.h"
#include "Utils.h"
#include <iostream>
#include <regex>


void UHPInterface::initTerminal()
{
    std::string input;
    std::vector<std::string> tokens;

    // Terminal loop
    while (true)
    {
        std::getline(std::cin, input);
        input = Utils::strip(input);
        tokens = Utils::tokenize(input, ' ');
        
        // TODO: Length check
        if (tokens[0] == "info") { _info(input); }
        else if (tokens[0] == "play") { _play(input); }
        else if (tokens[0] == "bestmove") { _bestMove(input); }
        else if (tokens[0] == "undo") { _undo(input); }
        else if (tokens[0] == "pass") { _pass(input); }
        else if (tokens[0] == "exit") { exit(0); }
        else
        {
            if (tokens[0].size() > 0)
            {
                std::cout << "Unknown command " << tokens[0] << std::endl;
                std::cout << "ok" << std::endl;
            };
            continue;
        };
    };  
};

void UHPInterface::_info(std::string input)
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');

    std::cout << "id hive++ indev" << std::endl;

    if (tokens.size() > 1)
    {
        std::cout << "Note: Arguments discarded. info takes no arguments." << std::endl;
    };

    std::cout << "ok" << std::endl;  
};

void UHPInterface::_play(std::string input)
{
    std::string cleanInput = Utils::strip(input);
    cleanInput = cleanInput.substr(5);
    std::vector<std::string> tokens = Utils::tokenize(cleanInput, ';');

    if (tokens.size() != 1)
    {
        std::cout << "Too many arguments. Usage: play [MoveString/pass]" << std::endl;
    }
    else
    {
        std::string second = Utils::strip(tokens[0]);
        if (second == "pass")
        { 
            _pass("pass");
            return;
        }
        else if (Utils::isMoveString(second))
        {
            // check for validity of move in Board object
            _game.makeMove(second);
        }
        else
        {
            std::cout << "Argument not recognized. Usage: play [MoveString/pass]" << std::endl;
        };
    };
    
    std::cout << _game.toString() << std::endl;
    std::cout << "ok" << std::endl;
};


void UHPInterface::_bestMove(std::string input)
// This will eventually need to be expanded to account for depth and time arguments
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');

    if (tokens.size() > 1)
    {
        std::cout << "Note: Arguments discarded. bestmove takes no arguments." << std::endl;
    };

    Move recc = _game.recommendMove();
    std::cout << recc.toString() << std::endl;
    std::cout << "ok" << std::endl;
};


void UHPInterface::_undo(std::string input)
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');

    if (tokens.size() > 2)
    {
        std::cout << "Too many arguments. Usage: undo [int numMoves]" << std::endl;
    }
    else if (tokens.size() == 1)
    {
        // Need to account for what happens if an undo is not possible 
        _game.undoLast();
        std::cout << _game.toString() << std::endl;
    }
    else // one argument was given
    {
        try
        {
            int numMoves = std::stoi(Utils::strip(tokens[0]));
            for (int i = 0; i < numMoves; i++)
            {
                _game.undoLast();
            };
            std::cout << _game.toString() << std::endl;
        }
        catch (std::invalid_argument &e)
        {
            std::cout << "Invalid argument. Usage undo [int numMoves]" << std::endl;
        };
    }
    std::cout << "ok" << std::endl;
};


void UHPInterface::_pass(std::string input)
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');
    if (tokens.size() != 1)
    {
        std::cout << "Too many arguments. pass takes no arguments." << std::endl;
    }
    else
    {
        Move recc = _game.recommendMove();
        _game.makeMove(recc);
    };
    std::cout << _game.toString() << std::endl;
    std::cout << "ok" << std::endl;
};




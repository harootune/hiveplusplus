#include "Interface.h"
#include "Utils.h"
#include <iostream>
#include <regex>


void UHPInterface::initTerminal()
{
    std::string input;
    std::vector<std::string> tokens;

    while (true)
    {
        std::getline(std::cin, input);
        input = Utils::strip(input);
        tokens = Utils::tokenize(input, ';');
        
        // TODO: Length check
        if (tokens[0] == "info") { _info(tokens); }
        else if (tokens[0] == "play") { _play(tokens); }
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

void UHPInterface::_info(std::vector<std::string> tokens)
{
    std::cout << "id hive++ indev" << std::endl;

    if (tokens.size() > 1)
    {
        std::cout << "Note: Arguments discarded. info takes no arguments." << std::endl;
    };

    std::cout << "ok" << std::endl;  
};

void UHPInterface::_play(std::vector<std::string> tokens)
{
    if (tokens.size() != 2)
    {
        std::cout << "Too many arguments. Usage: play [MoveString/pass]" << std::endl;
    }
    else
    {
        std::string second = Utils::strip(tokens[1]);
        if (second == "pass")
        { 
            // implement passing behavior
            std::cout << "Passing..." << std::endl;
        }
        else if (Utils::isGameString(second))
        {
            // check for validity of move in Board object
            _game.makeMove(second);
        }
        else
        {
            std::cout << "Argument not recognized. Usage: play [MoveString/pass]" << std::endl;
        };
    };
    
    std::cout << "ok" << std::endl;
};

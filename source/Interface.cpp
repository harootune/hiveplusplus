#include <Interface.h>
#include <Utils.h>
#include <ConfigReader.h>
#include <iostream>
#include <regex>

std::map<int, int> StandardPConfigs::base
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


void UHPInterface::initTerminal()
{
    std::string input;
    std::vector<std::string> tokens;

    // Initial output
    _info("info");

    // Terminal loop
    while (true)
    {
        std::getline(std::cin, input);
        input = Utils::strip(input);
        tokens = Utils::tokenize(input, ' ');
        
        // TODO: Length check
        if (tokens[0] == "info") { _info(input); }
        else if (tokens[0] == "play") { _play(input); }
        else if (tokens[0] == "validmoves") { _validMoves(input); } 
        else if (tokens[0] == "bestmove") { _bestMove(input); }
        else if (tokens[0] == "newgame") { _newGame(input); }
        else if (tokens[0] == "undo") { _undo(input); }
        else if (tokens[0] == "pass") { _pass(input); }
        else if (tokens[0] == "options") { _options(input); }
        else if (tokens[0] == "exit") { exit(0); }
        else
        {
            if (tokens[0].size() > 0)
            {
                std::cout << "err Unknown command " << tokens[0] << std::endl;
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
        std::cout << "note Arguments discarded. info takes no arguments." << std::endl;
    };

    std::cout << "ok" << std::endl;  
};

void UHPInterface::_play(std::string input)
{
    if (_active)
    {
        std::string cleanInput = Utils::strip(input);
        cleanInput = cleanInput.substr(5);
        std::vector<std::string> tokens = Utils::tokenize(cleanInput, ';');

        if (tokens.size() != 1)
        {
            std::cout << "err Too many arguments. Usage: play [MoveString/pass]" << std::endl;
        }
        else
        {
            if  (_game.gamestate > 1)
            {
                std::cout << "err Game in end state. Undo or start a new game to continue." << std::endl;
            }
            else
            {
                std::string second = Utils::strip(tokens[0]);
                if (Utils::isMoveString(second))
                {
                    LabelMove checkMove = _game.stringToMove(second);
                    LabelMove *refMove = _game.validateMove(&checkMove);

                    if (refMove != nullptr)
                    {
                        _game.makeMove(checkMove);
                    }
                    else
                    {
                        std::cout << "err Illegal move passed. Converted form: " << checkMove.toString() << std::endl;
                    };
                }
                else
                {
                    std::cout << "err Argument not recognized. Usage: play [MoveString/pass]" << std::endl;
                };
            };
        };
        std::cout << _mode << ";" << _game.toString() << std::endl;
    }
    else
    {
        std::cout << "err No active game. Use newgame to initialize a game." << std::endl;
    };

    std::cout << "ok" << std::endl;
};


void UHPInterface::_bestMove(std::string input)
{
    if (_active)
    {
        if  (_game.gamestate > 1)
        {
            std::cout << "err Game in end state. No move recommendation possible." << std::endl;
        }
        else
        {
            std::vector<std::string> tokens = Utils::tokenize(input, ' ');

            if (tokens.size() > 3)
            {
                std::cout << "err Invalid argument number. Usage: bestmove (depth [maxDepth] | time [HH:MM:SS duration])?" << std::endl;
                return;
            }
            else if (tokens.size() == 1)
            {
                LabelMove recc = _game.recommendMove();
                std::cout << recc.toString() << std::endl;
            }
            else
            {
                if (tokens[1] == "depth")
                {
                    try
                    {
                        int depth = std::stoi(tokens[2]);

                        if (depth < 1)
                        {
                            std::cout << "err Depth of search must be at least 1." << std::endl;
                        }
                        else
                        {
                            LabelMove recc = _game.recommendMove(depth);
                            std::cout << recc.toString() << std::endl;
                        };
                    }
                    catch (std::invalid_argument &e)
                    {
                        std::cout << "err Depth of search must be an integer." << std::endl;
                    };
                }
                else if (tokens[1] == "time")
                {
                    if (Utils::isTimeString(tokens[2]))
                    {
                        int duration = Utils::extractSeconds(tokens[2]);

                        if (duration < 1)
                        {
                            std::cout << "err Time of search must be at least 1 second." << std::endl;
                        }
                        else
                        {
                            LabelMove recc = _game.recommendMove(_defaultDepth, duration);
                            std::cout << recc.toString() << std::endl;
                        };
                    }
                    else
                    {
                        std::cout << "err Argument to time must be a TimeString with format HH:MM:SS." << std::endl;
                    };
                }
                else
                {
                    std::cout << "err Subcommand " << tokens[1] << " not recognized." << std::endl;
                };
            };
        };
    }
    else
    {
        std::cout << "err No active game. Use newgame to start a new game." << std::endl;
    };

    std::cout << "ok" << std::endl;
};


void UHPInterface::_undo(std::string input)
{
    if (_active)
    {
        std::vector<std::string> tokens = Utils::tokenize(input, ' ');

        if (tokens.size() > 2)
        {
            std::cout << "err Too many arguments. Usage: undo [int numMoves]" << std::endl;
        }
        else if (tokens.size() == 1)
        {
            // Need to account for what happens if an undo is not possible 
            _game.undoLast();
            std::cout << _mode << ";" << _game.toString() << std::endl;
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
                std::cout << _mode << ";" << _game.toString() << std::endl;
            }
            catch (std::invalid_argument &e)
            {
                std::cout << "err Invalid argument. Usage undo [int numMoves]" << std::endl;
            };
        };
    }
    else
    {
        std::cout << "err No active game. Use newgame to initialize a game." << std::endl;
    };
    std::cout << "ok" << std::endl;
};


void UHPInterface::_pass(std::string input)
// TODO REWRITE
{
    std::vector<std::string> tokens = Utils::tokenize(input, ' ');
    if (tokens.size() != 1)
    {
        std::cout << "err Too many arguments. pass takes no arguments." << std::endl;
    }
    else
    {
        _play("play pass");
    };
};


void UHPInterface::_validMoves(std::string input)
{
    if (_active)
    {
        std::vector<std::string> tokens = Utils::tokenize(input, ' ');
        if (tokens.size() != 1)
        {
            std::cout << "err Too many arguments. pass takes no arguments." << std::endl;
        }
        else
        {
            if  (_game.gamestate > 1)
            {
                std::cout << "err Game in end state. Undo or start a new game to continue." << std::endl;
            }
            else
            {
                std::string repr = "";
                std::vector<LabelMove> moves = _game.genAllMoves();

                for (LabelMove m: moves)
                {
                    repr += m.toString() + ";";
                };

                repr = repr == "" ? "No valid moves" : repr;
                std::cout << repr << std::endl;
            };
        };
    }
    else
    {
        std::cout << "err No active game. Use newgame to initialize a game." << std::endl;
    };
    std::cout << "ok" << std::endl;
};


void UHPInterface::_newGame(std::string input)
{
    // This is going to cause a problem if someone provides an invalid game string
    // TODO improve safety
    std::vector<std::string> engineHistory = _game.history;  // this is to help with reversability, currently does nothing
    LabelMove checkMove;
    LabelMove *refMove;

    if (input.size() > 8)
    {
        std::string cleanInput = Utils::strip(input.substr(8));

        if (Utils::isGameString(cleanInput))
        {
            std::vector<std::string> tokens = Utils::tokenize(cleanInput, ';');
            bool check;

            check = _initGame(tokens[0]);

            if (check && tokens.size() > 3)
            {
                std::vector<std::string>::iterator tokenIt = tokens.begin() + 3;
                for (; tokenIt != tokens.end() ; tokenIt++)
                {
                    checkMove = _game.stringToMove(*tokenIt);
                    refMove = _game.validateMove(&checkMove);

                    if (refMove != nullptr)
                    {
                        _game.makeMove(checkMove);
                    }
                    else
                    {
                        std::cout << "err Invalid move detected in GameString, reverting moves" << std::endl;
                        _game.reset(); // another place where reversability must be implemented
                        break;
                    };
                };
            };
        }
        else if (Utils::isGameTypeString(cleanInput))
        {
            _initGame(cleanInput);
        }
        else
        {
            std::cout << "err Not a valid GameString." << std::endl;
        };
    }
    else
    {
        _initGame("Base");
    };

    if (_active)
    {   
        std::cout << _mode << ";" << _game.toString() << std::endl;
    };
    
    std::cout << "ok" << std::endl;
};

void UHPInterface::_options(std::string input)
{
    input = Utils::strip(input);
    
    if (input.size() > 7)
    {
        input = Utils::strip(input.substr(7));
        std::vector<std::string> tokens = Utils::tokenize(input, ' ');
        if (tokens[0] == "get")
        {
            if (tokens.size() == 2)
            {
                if (tokens[1] == "CustomPath")
                {
                    std::cout << "CustomPath;string;" << _customPath << std::endl;
                } 
                else if (tokens[1] == "MaxTableSize")
                {
                    std::cout << "MaxTableSize;int;" << _tableSize << ";16;" << std::endl;
                }
                else if (tokens[1] == "DefaultDepth")
                {
                    std::cout << "DefautlDepth;int;" << _defaultDepth << ";1;" << std::endl;
                }
                else
                {
                    std::cout << "err Option name not recognized." << std::endl;
                };
            }
            else
            {
                std::cout << "err get takes one argument. Usage: options get [optionName]" << std::endl;
            };
        }
        else if (tokens[0] == "set")
        {
            if (tokens.size() == 3)
            {
                if (tokens[1] == "CustomPath")
                {
                    _customPath = tokens[2];
                    std::cout << "CustomPath;string;" << _customPath << std::endl;
                }
                else if (tokens[1] == "MaxTableSize")
                {
                    try
                    {
                        int mb = std::stoi(tokens[2]);

                        if (mb < 16)
                        {
                            std::cout << "err Minimum table size: 16MB" << std::endl;
                        }
                        else
                        {
                            _tableSize = mb;
                            if (_active)
                            {
                                _game.setTableSize(mb * 1048576);
                            };
                        };
                    }
                    catch (std::invalid_argument &e)
                    {
                        std::cout << "err Non-integer input." << std::endl; 
                    };
                }
                else if (tokens[1] == "DefaultDepth")
                {
                    try
                    {
                        int depth = std::stoi(tokens[2]);

                        if (depth < 1)
                        {
                            std::cout << "err Minimum default depth: 1 ply" << std::endl;
                        }
                        else
                        {
                            _defaultDepth = depth;
                        };
                    }
                    catch (std::invalid_argument &e)
                    {
                        std::cout << "err Non-integer input." << std::endl; 
                    };
                }
                else
                {
                    std::cout << "err Option name not recognized." << std::endl;
                };
            }
            else
            {
                std::cout << "err set takes two arguments. Usage: options set [optionName] [value]" << std::endl;
            };
        }
        else
        {
            std::cout << "err options subcommand not recognized. Available subcommands: get, set" << std::endl;
        };
    }
    else
    {
        // CustomPath
        std::cout << "CustomPath;string;" << _customPath << std::endl; 
        // MaxTableSize
        std::cout << "MaxTableSize;int;" << _tableSize << ";16;" << std::endl;
        // DefaultDepth
        std::cout << "DefaultDepth;int;" << _defaultDepth << ";1;" << std::endl;
    };
    std::cout << "ok" << std::endl;
};

bool UHPInterface::_initGame(std::string input)
{
    if (input == "Base")
    {
        _active = true;
        _mode = "Base";
        _game = Engine(StandardPConfigs::base);
    }
    else if (input == "Custom")
    {
        std::map<int, int> custom = ConfigReader::readConfig(_customPath);
        bool check = ConfigReader::validateConfig(custom);

        if (check)
        {
            _active = true;
            _mode = "Custom";
            _game = Engine(custom);
        }
        else
        {
            std::cout << "err Path to custom piece config was not found or is not valid." << std::endl;
            return false;
        };
    }
    else
    {
        // This is temporary handling for expansion pieces, will not be relevant later
        std::cout << "err GameType not recognized." << std::endl;
        return false;
    };

    if (_active)
    {
        _game.setTableSize(_tableSize * 1048576);
    };

    return true;
};



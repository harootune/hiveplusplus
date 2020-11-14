#include "Utils.h"
#include "PieceInfo.h"
#include <regex>
#include <cctype>

std::vector<std::string> Utils::tokenize(std::string input, char delimiter)
{
    std::vector<std::string> result;
    std::string current = "";

    for (char el: input)
    {
        if (el == delimiter)
        {
            result.push_back(current);
            current = "";
        }
        else
        {
            current += el;
        };
    };

    result.push_back(current);
    return result;
};

std::string Utils::strip(std::string input)
{
    std::string::iterator inputIt = input.begin();
    std::string output = "";
    std::string spaceBuffer = "";

    // iterate to first non-wspace character
    while (std::isspace(*inputIt))
    {
        inputIt++;
        if (inputIt == input.end()) { return output; };
    };

    for (inputIt; inputIt != input.end(); inputIt++)
    {
        if (std::isspace(*inputIt))
        {
            spaceBuffer += *inputIt;
        }
        else
        {
            if (spaceBuffer.size() > 0)
            {
                output += spaceBuffer;
                spaceBuffer = "";
            };

            output += *inputIt;
        };
    };

    return output;
};

bool Utils::isMoveString(std::string input)
{
    // could be static somewhere
    std::regex pattern("([wb][ABGQWS]([1-9]?[0-9]?)?)(\\s*(([\\\\/-][wb][ABGQWS]([1-9]?[0-9]?)?)|([wb][ABGQWS]([1-9]?[0-9]?)?[\\\\/-])))?");

    return std::regex_match(input, pattern);
};

bool Utils::isGameTypeString(std::string input)
{
    std::regex pattern("(Custom)|(Base(\\+[MLP][MLP]?[MLP]?)?)");

    return std::regex_match(input, pattern);
};

bool Utils::isGameStateString(std::string input)
{
    std::regex pattern("(NotStarted)|(Inprogress)|(Draw)|(WhiteWins)|(BlackWins)");

    return std::regex_match(input, pattern);
};

bool Utils::isTurnString(std::string input)
{
    std::regex pattern ("(White|Black)\\[[0-9]\\+\\]");

    return std::regex_match(input, pattern);
};

bool Utils::isGameString(std::string input)
{
    std::vector<std::string> tokens = tokenize(input, ';');

    if (tokens.size() < 3) { return false; };

    bool typeStringCheck = isGameTypeString(tokens[0]);
    bool stateStringCheck = isGameStateString(tokens[1]);
    bool turnStringCheck = isTurnString(tokens[2]);
    bool moveStringCheck = true;

    if (tokens.size() > 3)
    {
        for (std::string token: tokens)
        {
            if (!isMoveString(token))
            {
                moveStringCheck = false;
                break;
            };
        };
    };

    return typeStringCheck && stateStringCheck && turnStringCheck && moveStringCheck;
};

int Utils::labelToCode(std::string label)
{
  std::string prefix = label.substr(0, 2);
  return LabelCodes[prefix];  
};

std::vector<int> Utils::concatCoords(std::vector<int> coords, int piece)
{
    std::vector<int> tempCoords = coords;
    tempCoords.push_back(piece);
    
    return tempCoords;
};
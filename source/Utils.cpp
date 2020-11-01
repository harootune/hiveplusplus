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

std::string strip(std::string input)
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

bool Utils::isGameString(std::string input)
{
    // could be static somewhere
    std::regex pattern("([w,b][A,B,G,Q,W])(\s*([\\,/,-][w,b][A,B,G,Q,W]|[w,b][A,B,G,Q,W][\\,/,-]))?");
    
    return std::regex_match(input, pattern);
};

int Utils::labelToCode(std::string label)
{
  std::string prefix = label.substr(0, 2);
  return LabelCodes[prefix];  
};
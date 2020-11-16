#include <Utils.h>
#include <PieceInfo.h>
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

    for (; inputIt != input.end(); inputIt++)
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
    std::regex pattern("(NotStarted)|(InProgress)|(Draw)|(WhiteWins)|(BlackWins)");

    return std::regex_match(input, pattern);
};

bool Utils::isTurnString(std::string input)
{
    std::regex pattern ("(White|Black)\\[[0-9]+\\]");

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
        std::vector<std::string>::iterator tokenIt = tokens.begin() + 3;
        for (; tokenIt != tokens.end(); tokenIt++)
        {
            if (!isMoveString(*tokenIt))
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

PositionMove Utils::toPositionMove(LabelMove &labelMove, Board &board)
{
    Piece *toTarget;
    std::vector<int> fromCoords;

    if (labelMove.newPiece)
    {
        if (labelMove.firstPiece)
        {
            return PositionMove(labelMove.code);
        }
        else
        {
            fromCoords = {-1, -1, -1, -1};
        };
    }
    else
    {
        Piece *fromTarget = board.find(labelMove.from);
        
        if (fromTarget == nullptr)
        {
            return PositionMove();
        }
        else
        {
            fromCoords = fromTarget->getCoords();
        };
    };

    toTarget = board.find(labelMove.to);

    if (toTarget == nullptr)
    {
        return PositionMove();
    };

    std::vector<int> destCoords = board.top(toTarget->getNeighbor(labelMove.direction));
    if (board.find(destCoords) != nullptr)
    {
        destCoords[3]++;
    };

    return PositionMove(labelMove.code, fromCoords, destCoords, labelMove.newPiece);  
};

LabelMove Utils::toLabelMove(PositionMove &positionMove, Board &board)
{
    std::string label;
    int direction;
    Piece *toTarget;
    Piece *fromTarget;
    Position destPosition;
    std::vector<std::vector<int>> neighbors;

    // if this is a new piece, do this
    if (positionMove.newPiece)
    {
        // if this is a first piece, do this
        if (positionMove.firstPiece)
        {
            // return a firstpiece move of this piece type
            return LabelMove(board.nextLabel(positionMove.code));
        }
        else
        {
            // otherwise, set fromTarget to null and get the next label for this code type
            fromTarget = nullptr;
            label = board.nextLabel(positionMove.code);
        };
    }
    // if this is a piece translation, do this
    else
    {
        // fromTarget should be a piece on the board
        fromTarget = board.find(positionMove.from);
        if (fromTarget == nullptr)
        {
            return LabelMove(); // if not, return nonmove
        };

        // otherwise, label is set to the label of fromTarget
        label = fromTarget->label;
    };
    
    if (board.find(positionMove.to) != nullptr) // is this check appropriate?
    {
        return LabelMove();
    };

    // Create a position object at the destination point and get its adjacencies
    destPosition = Position(positionMove.to);
    neighbors = board.adjacencies(&destPosition);

    // if there are no occupied adjacencies, this is impossible, so return a nonmove
    if (neighbors.empty())
    {
        return LabelMove();
    };

    // toTarget is a nullptr by default
    toTarget = nullptr;

    // for every adjacency
    for (std::vector<int> n: neighbors)
    {
        // find the current neighbor
        toTarget = board.find(n);
        
        // if the found neighbor is our fromTarget, reset toTarget and move on
        if (toTarget == fromTarget)
        {
            toTarget = nullptr;
        }
        // otherwise break out of the loop
        else
        {
            break;
        };
    };

    // if we didnt find any non-toTarget neighbors, this move is impossible, return nonmove
    if (toTarget == nullptr)
    {
        return LabelMove();
    };

    // find direction
    direction = Position::findDirection(toTarget->getCoords(), destPosition.getCoords());

    return LabelMove(label, toTarget->label, direction, positionMove.newPiece);
};
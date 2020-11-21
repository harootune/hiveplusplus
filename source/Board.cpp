#include <Board.h>
#include <PieceInfo.h>
#include <Score.h>
#include <Utils.h>
#include <stdexcept>
#include <cmath>


_OneHiveInfo::_OneHiveInfo(): time(0) {};


Board::Board()
{
    count = 0;
};


Piece *Board::find(std::string label)
{
    try
    {
        return _labelToPiece.at(label);
    }
    catch (std::out_of_range &e)
    {
        return nullptr;
    };
};


Piece *Board::find(std::vector<int> coords)
{
    try
    {
        std::map<int, Piece*> inner = _coordsToPiece.at(
            {coords[0], coords[1], coords[2]}
        );
        return inner.at(coords[3]);
    }
    catch (std::out_of_range &e)
    {
        return nullptr;
    };
};


std::vector<int> Board::top(std::vector<int> coords)
// This returns the coordinates of the highest stacked piece at a certain x,y,z coordinate, or
// x,y,z,0 if no piece is present.
{
    std::vector<int> outerCoords;

    outerCoords.insert(outerCoords.begin(), coords.begin(), coords.begin()+3);

    try
    {
        std::map<int, Piece*> inner = _coordsToPiece.at(outerCoords);
        if (!inner.empty())
        {
            std::map<int, Piece*>::reverse_iterator topPiece = inner.rbegin();
            for (; topPiece != inner.rend(); topPiece++)
            {
                if (topPiece->second != nullptr)
                {
                    outerCoords.push_back(topPiece->first);
                    return outerCoords;
                };
            };
            outerCoords.push_back(0);
            return outerCoords;
        }
        else
        {
            outerCoords.push_back(0);
            return outerCoords;
        }
    }
    catch (std::out_of_range &e)
    {
        outerCoords.push_back(0);
        return outerCoords;
    };
};


std::vector<std::vector<int>> Board::adjacencies(std::string label, bool empty)
{
    Piece *neighbor;

    Piece *target = find(label);
    std::vector<std::vector<int>> existingNeighbors;
    std::vector<std::vector<int>> neighborCoords = target->getAllNeighbors();
    
    for (int i = 0; i < 6; i++)
    {
        neighborCoords[i] = top(neighborCoords[i]);
        neighbor = find(neighborCoords[i]);
        if ((neighbor != nullptr && !empty) || // If not empty and there is a neighbor
            (neighbor == nullptr && empty)) // or empty and there is no neighbor, add this coordinate
        {
            existingNeighbors.push_back(neighborCoords[i]);
        };
    };

    return existingNeighbors;
};


std::vector<std::vector<int>> Board::adjacencies(Position *pos, bool empty)
{
    Piece *neighbor;
    std::vector<std::vector<int>> existingNeighbors;

    std::vector<std::vector<int>> neighborCoords = pos->getAllNeighbors();
    
    for (int i = 0; i < 6; i++)
    {
        neighborCoords[i] = top(neighborCoords[i]);
        neighbor = find(neighborCoords[i]);
        if ((neighbor != nullptr && !empty) ||
            (neighbor == nullptr && empty))
        {
            existingNeighbors.push_back(neighborCoords[i]);
        };
    };

    return existingNeighbors;
};


Piece *Board::getFirst()
// this is primarily used to get a random piece as a start point for certain traversals
// e.g. articulation vertex search
{
    if (empty())
    {
        return nullptr;
    }
    else
    {
        Piece *target = _labelToPiece.begin()->second;
        return target;
    };
};


std::vector<Piece*> Board::getColorPieces(bool white)
// TODO: Find a better way to extract color pieces besides the character key
// Maybe maintain sets of each color's pieceS?
{
    std::vector<Piece*> targets;
    std::map<std::string, Piece*>::iterator labelIt;

    char key = white ? 'w' : 'b';

    // For each piece in the _labelToPiece table
    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {
        // if the label begins with the corresponding key
        if (labelIt->first[0] == key)
        {
            // Add it to our accumulator
            targets.push_back(labelIt->second);
        };
    };

    return targets;
};


std::vector<Piece*> Board::getAllPieces()
{
    std::map<std::string, Piece*>::iterator labelIt;
    std::vector<Piece*> targets;

    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {

        targets.push_back(labelIt->second);

    };

    return targets;   
};


std::set<std::vector<int>> Board::getPinned()
{
    // return an empty set if empty, as Board::_pinSearch cannot execute without a starting location
    if (empty())
    {
        return {};
    };

    _OneHiveInfo info;

    std::vector<int> rootParent {-1, -1, -1, -1};
    std::vector<int> start = getFirst()->getCoords();
    start[3] = 0;

    // this will break if there are no pieces on the board
    _pinSearch(rootParent, start, &info);

    return info.articulations; // the _OneHiveInfo object accumulates articulation vertices - return those
};


void Board::_pinSearch(std::vector<int> &parent, std::vector<int> &location, _OneHiveInfo *info)
// This is an implementation of a linear-time articulation vertext discovery algorithm described here:
// https://cp-algorithms.com/graph/cutpoints.html
{
    Position current(location);

    int children = 0; // child count of current node
    std::vector<int> rootParent {-1, -1, -1, -1};

    // update the _OneHiveInfo structure
    info->time++;
    info->visited[location] = true;
    info->entryTime[location] = info->time;
    info->earliestTime[location] = info->time;
    
    // iterate through every adjacency
    std::vector<std::vector<int>> adj = adjacencies(&current);
    for (std::vector<int> a: adj)
    {
        a[3] = 0; // bottom out the adjacency -- a little janky but works for now

        if (a != parent) // ignore the edge back to the parent
        {
            if (info->visited[a]) // this is a back edge
            {
                // earliest we can get back to is the entry time of the back node
                info->earliestTime[location] = std::min(info->earliestTime[location], info->entryTime[a]);
            }
            else // this is a tree edge
            {  
                // recur the dfs
                _pinSearch(location, a, info);

                // earliest we can get back to is the earliest node the child can get to
                info->earliestTime[location] = std::min(info->earliestTime[location], info->earliestTime[a]);

                // if we can go no further back then the current node, this is an articulation vertex
                if (info->earliestTime[a] >= info->entryTime[location] && parent != rootParent)
                {
                    info->articulations.insert(location);
                }
                children++;
            };
        };
    };

    // finally, if this is the root and it has more than one child, the root itself is an articulation vertex
    if (parent == rootParent && children > 1)
    {
        info->articulations.insert(location);
    };
};


std::vector<int> Board::getCenter()
// This returns an integer hex coordinate coordinate close to the true centroid of the board
// It does not necessarily return the closest coordinate to the centroid due to flooring effect of integer division
// only returns x,y,z <- TODO: Should this return a full xyzv coordinate?
{
    // throw an error if the board is empty
    if (empty())
    {
        throw std::runtime_error("Cannot find the center of an empty board");
    };

    int i;
    std::vector<int> centroid {0, 0, 0};
    std::vector<Piece*> pieces;

    // accumulate all coordinate values in the centroid
    for (Piece *piece: pieces)
    {
        for (i = 0; i < 2; i++)
        {
            centroid[i] += piece->getCoords()[i];
        };
    };

    // divide x and y by the number of coordinates accumulated, then adjust z to ensure validity
    centroid[0] /= count;
    centroid[1] /= count;
    centroid[2] = -(centroid[0] + centroid[1]);

    return centroid;

};


int Board::getRadius()
// the radius here is defined as the longest hex-tile distance between the origin and any other piece on the board
{
    int radius = 0;
    Position origin({0, 0, 0, 0});

    for (Piece *piece: getAllPieces())
    {
        radius = std::max(radius, origin.findDistance(piece));
    };

    return radius;
};


void Board::update(LabelMove &move, bool reversible)
{
    Piece *target;
    Piece *underPiece;
    std::vector<int> outer;
    std::vector<int> newCoords;
    std::vector<int> underCoords;
    
    // If this is a move forward in time, store a move which lets us reverse it
    if (reversible)
    {
        _storeUndo(move);
    };

    // we don't need to do anything for a pass move
    if (!move.pass)
    {
        // if this is not a new piece placement (is a normal move), do this
        if (!move.newPiece)
        {
            std::vector<int> oldCoords;

            // find the starting and ending coordinates of the move
            target = find(move.from);
            oldCoords = target->getCoords();
            newCoords = find(move.to)->getNeighbor(move.direction);
            newCoords = top(newCoords);
            if (find(newCoords) != nullptr)
            {
                newCoords[3]++; // if we are moving to a location where a piece is already present, increment v by 1
            };
            target->setCoords(newCoords);

            // add the new coordinate mapping
            outer = {newCoords[0], newCoords[1], newCoords[2]};
            _coordsToPiece[outer][newCoords[3]] = target;
            
            // erase old coordinate mapping
            outer = {oldCoords[0], oldCoords[1], oldCoords[2]};
            // this leaves behind the x ,y, z key but breaks searches to full coords by removing v
            _coordsToPiece[outer].erase(oldCoords[3]);

            // track topping
            if (newCoords[3] > 0)
            {
                underCoords = newCoords;
                underCoords[3]--;
                underPiece = find(underCoords);

                // if we could not find an underPiece, we are attempting to move a piece floating in midair, so throw an error
                if (underPiece == nullptr)
                {
                    throw (std::runtime_error("No piece found beneath elevated piece: " + move.from));
                }
                else
                {
                    underPiece->isTopped = true;
                };
            };

            if (oldCoords[3] > 0)
            {
                underCoords = oldCoords;
                underCoords[3]--;
                underPiece = find(underCoords);

                // if we could not find an underPiece, we are attempting to move a piece floating in midair, so throw an error
                if (underPiece == nullptr)
                {
                    throw (std::runtime_error("No piece found beneath elevated piece: " + move.from));
                }
                else
                {
                    underPiece->isTopped = true;
                };
            };
        }
        // if this is a placement of a new piece, do this
        else
        {
            // if this is the first piece of the game, do this
            if (!move.firstPiece)
            {
                newCoords = find(move.to)->getNeighbor(move.direction);
                target = new Piece(newCoords, move.code, move.from);

                outer = {newCoords[0], newCoords[1], newCoords[2]};
                _coordsToPiece[outer][newCoords[3]] = target;

                _labelToPiece[move.from] = target;
            }
            // otherwise, do this
            else
            {
                newCoords = {0, 0, 0, 0};
                target = new Piece(newCoords, move.code, move.from);

                outer = {newCoords[0], newCoords[1], newCoords[2]};
                _coordsToPiece[outer][newCoords[3]] = target;

                _labelToPiece[move.from] = target;
            };

            count++;
            counts[move.code]++;

            // queen status update
            if (move.code == PieceCodes::wQ)
            {
                wQueen = true;
            }
            else if (move.code == PieceCodes::bQ)
            {
                bQueen = true;
            };
        };
    };
};


void Board::_storeUndo(LabelMove &move)
{   
    // if this is a piece placement or a pass move, store the move as is.
    // this will be used by Board::undoLast to remove the placed piece or ignore a pass
    if (move.newPiece || move.pass)
    {
        _undoCache.push_back(move);
    }
    // otherwise, if this is a regular move, store the reverse of the move to be passed to
    // Board::update by Board::undoLast
    else
    {
        Piece *target = find(move.from); 
        Piece *oldNeighbor = nullptr;

        bool found = false;

        for (int i = 0; i < 6; i++)
        {
            oldNeighbor = find(target->getNeighbor(i));
            
            if (oldNeighbor != nullptr)
            {
                int reverseDirection = (3 + i) % 6;
                LabelMove reverseMove(target->label, oldNeighbor->label, reverseDirection);
                _undoCache.push_back(reverseMove); // TODO: std::move?
                found = true;
                break;
            };   
        };

        // Throw an error if we were unable to find a reference point for the piece's original location
        if (!found) 
        { 
            throw(std::runtime_error("Cannot store reverse of a move without a reference point: " + move.toString()));
        };
    };
};


void Board::remove(std::string pieceLabel)
{
    Piece *target = find(pieceLabel);
    std::vector<int> coords = target->getCoords();
    std::vector<int> outer {coords[0], coords[1], coords[2]};

    // if this piece is elevated, remove update topping of piece below it
    if (coords[3] > 0)
    {
        std::vector<int> underCoords = coords;
        underCoords[3]--;
        Piece *underPiece = find(underCoords); // DEBUG: letting this fail for now when given junk underCoords
        underPiece->isTopped = false;
    };
    
    // this leaves behind the x ,y, z key but breaks searches to full coords by removing v
    _coordsToPiece[outer].erase(coords[3]);
    _labelToPiece.erase(pieceLabel);

    // update counts
    count--;
    counts[target->code]--;

    // update queen status
    if (pieceLabel == "wQ")
    {
        wQueen = false;
    }
    else if (pieceLabel == "bQ")
    {
        bQueen = false;
    };

    // delete the piece to prevent memory leakage
    delete target;
};


void Board::undoLast()
{
    // remember, undo moves are not the move history
    // they are either the reverse of a regular move or a placement move which provides a removal target
    LabelMove &undo = _undoCache.back();

    // don't do anything for pass moves, Engine will handle turn and player
    if (!undo.pass)
    {
        // if this is a new piece, we simply have to remove the piece it points to
        if (undo.newPiece)
        {
            remove(undo.from);
        } 
        // if this is a regular move, undo shows us how to reverse it
        else
        {
            update(undo, false); // undo moves themselves should not be placed in the _undoCache
        };
    };

    _undoCache.pop_back();
};


LabelMove Board::getLastUndo()
{
    if (_undoCache.empty())
    {
        return LabelMove(); // return a non-move if there's nothing in the undo cache
    }
    else
    {
        return _undoCache.back();
    };
};


void Board::recenter(std::vector<int> &centroid)
{
    int i;
    std::vector<int> outer;
    std::vector<int> coords;

    std::vector<Piece*> pieces = getAllPieces();

    // all existing coord->piece mappings are about to be invalidated, so clear them
    _coordsToPiece.clear();

    for (Piece *piece: pieces)
    {
        coords = piece->getCoords();
        for (i = 0; i < 3; i++)
        {
            coords[i] -= centroid[i]; // subtract the centroid from every piece's coordinate vector
        };
        piece->setCoords(coords);

        // update _coordsToPiece with the new coordinate
        outer = {coords[0], coords[1], coords[2]};
        _coordsToPiece[outer][coords[3]] = piece;
    }; 
};


bool Board::empty()
{
    return count == 0;
};


void Board::clear()
// flip the (proverbial, not literal) table
{
    _clearPieces();
    _labelToPiece.clear();
    _coordsToPiece.clear();
    _undoCache.clear();
    count = 0;
    counts.clear();
    wQueen = false;
    bQueen = false;
};


std::string Board::nextLabel(int code)
// return the next valid label for a given piece code on the board
// note that this doesn't inherently check against a piece configuration
{
    std::string label = PieceNames[code];

    if (code % 5 != 0)
    {
        label += std::to_string(counts[code] + 1);
    }
    return label;
};


int Board::checkGameState()
{
    // check for empty
    if (empty()) { return 0; };

    // else

    bool wCapture = false;
    bool bCapture = false;
    std::vector<std::vector<int>> adj;

    // if all of a queen's adjacencies are filled, set the corresponding capture flag
    if (wQueen)
    {
        adj = adjacencies("wQ");
        if (adj.size() == 6)
        {
            wCapture = true;
        };
    };

    if (bQueen)
    {
        adj = adjacencies("bQ");
        if (adj.size() == 6)
        {
            bCapture = true;
        };
    };

    // TODO: Figure out how to use the gamestate enum defined in Engine (should just move to Board.h)
    if (bCapture && wCapture)
    {
        return 2;
    }
    else if (bCapture)
    {
        return 3;
    }
    else if (wCapture)
    {
        return 4;
    }
    else
    {
        return 1;
    };
};


/* Board Scoring Constants and Functionality */

int Board::score(bool white)
{
    int sign = white ? 1 : -1;
    int gameState = checkGameState();
    std::set<std::vector<int>> pinned = getPinned();
    
    switch (gameState)
    {
        case 2:
            return Scores::drawScore;
        case 3:
            return sign * Scores::checkMateScore;
        case 4:
            return sign * -Scores::checkMateScore;
        default:
            break;
    };

    // no captures or draws

    int tempScore;
    int distance;
    std::vector<std::vector<int>> adj;
    int score = 0;
    std::vector<Piece*> pieces = getAllPieces();

    if (wQueen)
    {
         adj = adjacencies("wQ");

        // for every adjacent coordinate with a piece
        for (std::vector<int> el: adj) 
        {
            // if that piece is white, do this
            if (find(el)->code < PieceCodes::bQ)
            {
                // if the piece is white, +/- by 30
                score -= 30;
            }
            // otherwise
            else
            {
                // if the piece is black, +/- by 100 
                score -= 100;
            };
        };
    };

    if (bQueen)
    {
        adj = adjacencies("bQ");

        // for every adjacent coordinate with a piece
        for (std::vector<int> el: adj) 
        {
            // if that piece is white, do this
            if (find(el)->code < PieceCodes::bQ)
            {
                // if the piece is white, +/- by 100
                score += 100;
            }
            // otherwise
            else
            {
                // if the piece is black, +/- by 30
                score += 30;
            };
        };
    };
    
    for (Piece* p: pieces)
    {
        tempScore = 0;

        // if the piece is the same color as our current player, do this
        if (p->white)
        {
            // add base score
            tempScore += count < 10 ? Scores::baseScores[p->code % 5][count] : Scores::baseScores[p->code % 5][10];

            // offensive scores
            if (bQueen)
            {
                distance = p->findDistance(find("bQ"));
                if (distance < 3)
                {
                    tempScore += Scores::offScores[p->code % 5][distance];
                };
            };

            // defensive scores
            if (wQueen)
            {
                distance = p->findDistance(find("wQ"));
                if (distance < 3)
                {
                    tempScore += Scores::defScores[p->code % 5][distance];
                };
            };
        }
        else
        {
            tempScore -= count < 10 ? Scores::baseScores[p->code % 5][count] : Scores::baseScores[p->code % 5][10];

            if (wQueen)
            {
                distance = p->findDistance(find("wQ"));
                if (distance < 3)
                {
                    tempScore -= Scores::offScores[p->code % 5][distance];
                };
            };

            if (bQueen)
            {
                distance = p->findDistance(find("bQ"));
                if (distance < 3)
                {
                    tempScore -= Scores::defScores[p->code % 5][distance];
                };
            };
        };

        if (pinned.find(p->getCoords()) != pinned.end())
        {
            tempScore /= 2; // halve scores for pinned pieces
        };

        score += tempScore;
    };

    return sign * score;
};

void Board::_clearPieces()
{
    std::vector<Piece*> toDelete;
    std::vector<Piece*>::iterator deleteIt;
    std::map<std::string, Piece*>::iterator labelIt;

    for (labelIt = _labelToPiece.begin(); labelIt != _labelToPiece.end(); labelIt++)
    {
        toDelete.push_back(labelIt->second);
    };

    for (deleteIt = toDelete.begin(); deleteIt != toDelete.end(); deleteIt++)
    {
        delete *deleteIt;
    };
};

Board::~Board()
{
    _clearPieces();
};


/* DEBUGGING CODE BEYOND THIS POINT */

std::string Board::coordsMapToString()
{
    std::map<std::vector<int>, std::map<int, Piece*>>::iterator outerIt;
    std::map<int, Piece*>::iterator innerIt;
    std::string repr = "";

    for (outerIt = _coordsToPiece.begin(); outerIt != _coordsToPiece.end(); outerIt++)
    {
        for (innerIt = outerIt->second.begin(); innerIt != outerIt->second.end(); innerIt++)
        {
            if (innerIt->second != nullptr)
            {
                repr += " " + innerIt->second->label + " " + 
                    std::to_string(outerIt->first[0]) + "," +
                    std::to_string(outerIt->first[1]) + "," +
                    std::to_string(outerIt->first[2]) + "," +
                    std::to_string(innerIt->first) + ":";
            };
        };
    };

    return repr;
};










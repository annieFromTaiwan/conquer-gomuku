#ifndef FINDTHREAT_H
#define FINDTHREAT_H

#include <set>
#include <algorithm>
#include "err.h"
#include "BoardData.h"
#include "threat.h"

//#define RECORD_THREAT      // in findThreat(): [[__OOXX#___]], aabb

void printLine(vector <unsigned char> queue);

inline bool threatCmp(Threat *a, Threat *b)
{
    return (a->type) > (b->type);
}

/****************************************************************************/
//  specific square(EMPTY/turn) and specific direction,
//  return the only one threat(or null)
Threat* checkThreat(BoardData *brd,int turn,int x,int y,int dx,int dy,unsigned short int restMask);

// specific rest square(turn) with all direction,
// return all possible threats (with gain square EMPTY)
vector< Threat* > findAllThreatWithRest(BoardData *brd,int turn,int restX,int restY);

// returns the max one threat of
//    [turn]'s all threats (different direction)
//    on specific square [x, y] (could be EMPTY/turn)
Threat* findMaxThreat(BoardData *brd,int turn,int x,int y);

// returns the max one threat of [turn]'s all threats
// gainSquareIsEmpty specifies that if gain square is empty or not
// (gainSquareIsEmpty==true => gain square is EMPTY => the next-step threats of [turn])
Threat* findMaxThreat(BoardData *brd,int turn,bool gainSquareIsEmpty);
// (used in availableMove(),
// (uses findAllThreat(i, j))

// returns [turn]'s all threats that > minThreat
// the threat list is sorted
// (minThreat could be NULL, meaning that no threats are to be sieved)
// gainSquareIsEmpty specifies that if gain square is empty or not
// (gainSquareIsEmpty==true => gain square is EMPTY => the next-step threats of [turn])
vector< Threat* > findAllThreat(BoardData *brd, int turn, bool gainSquareIsEmpty, Threat *minThreat);
// (used in availableMove(), attackMove(), threatSpaceSearch(), proofNumberSearch())
// (uses checkThreat())

vector< pair<int,int> > availableMove(BoardData *brd, int turn);

vector< pair<int,int> > attackMove(BoardData *brd, int turn);
/****************************************************************************/

#endif // FINDTHREAT_H

//
//  PerformanceTest.hpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/7/21.
//

#pragma once

#include "BoardMoves.hpp"
#include "UCI.hpp"
#include <chrono>

#define InputBuffer (400 * 6)

class Search : public BoardMoves{
public:
    int pvArray[MaxDepth];
    SearchInfo *info = new SearchInfo();
private:
    long long nodes{};
    
    const int PawnTable[GameSquares] = {
    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,
    10   ,    10   ,    0    ,    -10  ,    -10  ,    0    ,    10   ,    10   ,
    5    ,    0    ,    0    ,    5    ,    5    ,    0    ,    0    ,    5    ,
    0    ,    0    ,    10   ,    20   ,    20   ,    10   ,    0    ,    0    ,
    5    ,    5    ,    5    ,    10   ,    10   ,    5    ,    5    ,    5    ,
    10   ,    10   ,    10   ,    20   ,    20   ,    10   ,    10   ,    10   ,
    20   ,    20   ,    20   ,    30   ,    30   ,    20   ,    20   ,    20   ,
    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,    0
    };

    const int KnightTable[GameSquares] = {
    0    ,    -10    ,    0    ,    0    ,    0    ,    0    ,    -10    ,    0    ,
    0    ,    0    ,    0    ,    5    ,    5    ,    0    ,    0   ,    0    ,
    0    ,    0    ,    10   ,    10   ,    10   ,    10   ,    0   ,    0    ,
    0    ,    0    ,    10   ,    20   ,    20   ,    10   ,    5   ,    0    ,
    5    ,    10   ,    15   ,    20   ,    20   ,    15   ,    10  ,    5    ,
    5    ,    10   ,    10   ,    20   ,    20   ,    10   ,    10  ,    5    ,
    0    ,    0    ,    5    ,    10   ,    10   ,    5    ,    0   ,    0    ,
    0    ,    0    ,    0    ,    0    ,    0    ,    0    ,    0   ,    0
    };

    const int BishopTable[GameSquares] = {
    0    ,    0    ,    -10  ,    0    ,    0    ,    -10    ,    0    ,    0    ,
    0    ,    0    ,    0    ,    10    ,    10    ,    0    ,    0    ,    0    ,
    0    ,    0    ,    10   ,    15    ,    15    ,    10   ,    0    ,    0    ,
    0    ,    10   ,    15   ,    20    ,    20    ,    15   ,    10   ,    0    ,
    0    ,    10   ,    15   ,    20    ,    20    ,    15   ,    10   ,    0    ,
    0    ,    0    ,    10   ,    15    ,    15    ,    10   ,    0    ,    0    ,
    0    ,    0    ,    0    ,    10    ,    10    ,    0    ,    0    ,    0    ,
    0    ,    0    ,    0    ,    0     ,    0     ,    0    ,    0    ,    0
    };

    const int RookTable[GameSquares] = {
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0    ,
    25   ,    25   ,    25   ,    25    ,    25    ,    25   ,    25   ,    25    ,
    0    ,    0    ,    5    ,    10    ,    10    ,    5    ,    0    ,    0
    };

    const int KingE[GameSquares] = {
        -50  ,    -10   ,    0    ,    0    ,    0    ,    0    ,    -10    ,    -50    ,
        -10,       0    ,    10    ,    10    ,    10    ,    10    ,    0    ,    -10    ,
        0    ,    10    ,    20    ,    20    ,    20    ,    20    ,    10    ,    0    ,
        0    ,    10    ,    20    ,    40    ,    40    ,    20    ,    10    ,    0    ,
        0    ,    10    ,    20    ,    40    ,    40    ,    20    ,    10    ,    0    ,
        0    ,    10    ,    20    ,    20    ,    20    ,    20    ,    10    ,    0    ,
        -10   ,    0    ,    10    ,    10    ,    10    ,    10    ,    0    ,    -10    ,
        -50    ,    -10    ,    0    ,    0    ,    0    ,    0    ,    -10    ,    -50
    };

    const int KingO[GameSquares] = {
        0    ,    5    ,    5    ,    -10    ,    -10    ,    0    ,    10    ,    5    ,
        -30    ,    -30    ,    -30    ,    -30    ,    -30    ,    -30    ,    -30    ,    -30    ,
        -50    ,    -50    ,    -50    ,    -50    ,    -50    ,    -50    ,    -50    ,    -50    ,
        -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,
        -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,
        -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,
        -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,
        -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70    ,    -70
    };
    
    const int MirrorBlack[GameSquares]{
        56,57,58,59,60,61,62,63,
        48,49,50,51,52,53,54,55,
        40,41,42,43,44,45,46,47,
        32,33,34,35,36,37,38,39,
        24,25,26,27,28,29,30,31,
        16,17,18,19,20,21,22,23,
        8 , 9,10,11,12,13,14,15,
        0 , 1, 2 ,3 ,4 ,5 ,6 ,7
    };

public:
    void perfTesting(int depth);
    void runTest(int depth);
    bool doesMoveExist(const int move);
    void storePvMove(const int move);
    int probePvTable();
    int getPvLine(const int depth);
    int evaluate();
    void PickNextMove(int moveNumber, MoveList *list);
    void clearSearch(SearchInfo *info);
    void searchPosition(SearchInfo *info);
    int AlphaBeta(int alpha, int beta, int depth, SearchInfo *info, bool nullMove);
    int quiescenceSearch(int alpha, int beta, SearchInfo *info);
    void checkStop(SearchInfo *info);
    ~Search();
};

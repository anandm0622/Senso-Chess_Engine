//
//  BoardMoves.hpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/2/21.
//

#pragma once

#include "BoardOperations.hpp"

struct BoardHistory{
    
    int lastMove;
    int castlePermission;
    int enPassantSquare;
    int fiftyMoveCounter;
    UINT64_t positionKey;
    
};

struct Move{
    int move;
    int score;
};

struct MoveList{
    Move moves[MaximumPositionMoves];
    int count;
};

struct SearchInfo{
    long long startTime;
    long long stopTime;
    int depth;
    int depthSet;
    int timeset;
    int movesLeft;
    bool infinite;
    long long nodes;
    bool quit;
    int stopped;
    float failHigh;
    float failHighFirst;
    int mullcut;
    
    int gameMode;
    int postThinking;
};

class BoardMoves : public BoardOperations{
    
public:
    int searchHistory[13][TotalSquares];    //everytime a move improves on alpha, that piecetype and the move sqaure will be incremented by 1
    int searchKillers[2][MaxDepth];         //stores two moves that caused beta cutoffs that wasn't a capture.
    BoardHistory history[MaximumGameMoves];

private:
    const int attackingPieceKnight[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
    const int attackingPieceRook[4] = {-1, -10, 1, 10};
    const int attackingPieceBishop[4] = {-9, -11, 9, 11};
    const int attackingPieceKing[8] = {-1, -10, -9, -11, 1, 10, 9, 11};
    const int slidingPieces[8] = {WhiteBishop, WhiteRook, WhiteQueen, 0, BlackBishop, BlackRook, BlackQueen, 0};
    const int nonSlidingPieces[6] = {WhiteKnight, WhiteKing, 0, BlackKnight, BlackKing, 0};
    const int slidingPieceIndex[2] = {0,4};
    const int nonSlidingPieceIndex[2] = {0,3};
    const int totalSquares[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};
    const int availableSquares[13][8] = {
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19, -21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10, 1, 10, 0, 0, 0, 0 },
        { -1, -10, 1, 10, -9, -11, 11, 9 },
        { -1, -10, 1, 10, -9, -11, 11, 9 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { -8, -19, -21, -12, 8, 19, 21, 12 },
        { -9, -11, 11, 9, 0, 0, 0, 0 },
        { -1, -10, 1, 10, 0, 0, 0, 0 },
        { -1, -10, 1, 10, -9, -11, 11, 9 },
        { -1, -10, 1, 10, -9, -11, 11, 9 }
    };
    const int CastlingBits[120] = {
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15, 15
    };
    
    //static data members
    static bool PieceSlides[13];
    static bool attackingSquarePawn[13];
    static bool attackingSquareKnight[13];
    static bool attackingSquareKing[13];
    static bool attackingSquareRook[13];
    static bool attackingSquareBishop[13];
        
    MoveList moveList;
    
public:
    std::string PrintMove(const int move);
    bool attackingSquare(const int square, const int attackingSide);
    void PrintMoveList();
    int parseMove(std::string move);
    void movePiece(const int from, const int to);
    bool makeMove(int move);
    void undoMove();
    bool isRepeated();
    void AddQuietMove(int move, MoveList* moveList);
    void AddCaptureMove(int move, MoveList* moveList);
    void AddEnPassantMove(int move, MoveList* moveList);
    void GenerateMoves(MoveList* moveList);
    void GenerateCaptureMoves(MoveList* moveList);
    void AddPawnMove(const int from, const int to, const int cap, MoveList* moveList);
//end
};

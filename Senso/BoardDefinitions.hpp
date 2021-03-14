//
//  BoardDefinitions.hpp
//  Senso
//
//  Created by Anand Manikandan on 2/27/21.
//

#pragma once

#define NDEBUG
#include <iostream>
#include <vector>
#include <random>
#include <assert.h>
#include <array>

#define Name "Senso v1.1"
#define Author "Anand Manikandan"
#define TotalSquares 120
#define GameSquares 64
#define MaximumGameMoves 2048       //2048 plys -> 1024 moves
#define MaximumPositionMoves 256    //Maximum moves for any given position
#define MaxDepth 64                 //maximum depth the engine will search
#define Infinite 30000

#define startingFEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define ISMATE (INFINITY - MaxDepth)
#define MATE 29000

//Bit Operations
#define FileRankToSquare(file, rank) ((21 +(file)) + ((rank) * 10))  //to convert a given file and rank to a sqaure on a 120 block board between 21 to 98
#define setBit(Bitboard, square) (Bitboard |= BoardDefinition::setBits[(square)])       //sets the given bit
#define clearBit(Bitboard, square) (Bitboard &= BoardDefinition::clearBits[(square)])       //clears the given bit

// Game Move Storage

/*
0000 0000 0000 0000 0000 0111 1111 -> set bits represent From square
0000 0000 0000 0011 1111 1000 0000 -> set bits represent To square
0000 0000 0011 1100 0000 0000 0000 -> set bits represent the Captured piece
0000 0000 0100 0000 0000 0000 0000 -> set bits represent if it is an en passant move
0000 0000 1000 0000 0000 0000 0000 -> set bits represent if it is a Pawn Start
0000 1111 0000 0000 0000 0000 0000 -> set bits represent the pawn promoted piece
0001 0000 0000 0000 0000 0000 0000 -> set bits represent if it was castling
*/
#define getFromSquare(move) ((move) & 0x7F)
#define getToSquare(move) ((move>>7) & 0x7F)
#define getCapturedPiece(move) ((move>>14) & 0xF)
#define getPromotedPiece(move) ((move>>20) & 0xF)
#define flagEnPassant 0x40000
#define flagPawnStart 0x80000
#define flagCapture 0x7C000
#define flagPromoted 0xF00000
#define flagCastliing 0x1000000


typedef unsigned long long UINT64_t;

enum chessPieces {EMPTY, WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing, BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing};
enum chessFile {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_NONE};
enum chessRanks {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE};
enum modes {UCIMODE, XBOARDMODE, CONSOLEMODE};
enum chessColors {WHITE, BLACK, BOTH};

enum castlingSides { WhiteKingSideCastling = 1, WhiteQueenSideCastling = 2, BlackKingSideCastling = 4, BlackQueenSideCastling = 8 };
    
enum chessSquares {
    A1 = 21, B1, C1, D1, E1, F1, G1, H1,
    A2 = 31, B2, C2, D2, E2, F2, G2, H2,
    A3 = 41, B3, C3, D3, E3, F3, G3, H3,
    A4 = 51, B4, C4, D4, E4, F4, G4, H4,
    A5 = 61, B5, C5, D5, E5, F5, G5, H5,
    A6 = 71, B6, C6, D6, E6, F6, G6, H6,
    A7 = 81, B7, C7, D7, E7, F7, G7, H7,
    A8 = 91, B8, C8, D8, E8, F8, G8, H8, NOSQUARE
};

template <typename T, size_t N>
constexpr size_t array_size(T (&)[N]) {
    return N;
}

struct PVEntry{
    UINT64_t positionKey;
    int move;
};

struct PVTable{
    PVEntry *table;
    int entries;
};

class BoardDefinition{
    
public:
    //static variables
    static int orientation120to64[TotalSquares];      //0 to 119 board
    static int orientation64to120[GameSquares];                 //64 block board of 0 to 63 which is 21 to 98 on the 120-board
    static UINT64_t setBits[GameSquares];
    static UINT64_t clearBits[GameSquares];
    static UINT64_t whitePassedPawns[GameSquares];
    static UINT64_t blackPassedPawns[GameSquares];
    static UINT64_t isolatedPawns[GameSquares];
    static UINT64_t BBFileMasks[8];
    static UINT64_t BBRankMasks[8];
    static UINT64_t pieceKeys[13][120];                    //Key for every piece by the total squares on the board
    static UINT64_t sideKey;                               //Hash a unique key if it is White's turn to move
    static UINT64_t castleKeys[16];                         //castling keys
    static int squareToFile[TotalSquares];
    static int squareToRank[TotalSquares];
    //pieces
    int pieces[TotalSquares];
    int pieceCount[13];         //number of each piece on the board
    int BigFivePieces[2];       //store the count for all the pieces except pawn for white, black and both sides on the board
    int majorPieces[2];         //count of Rooks and Queens
    int minorPieces[2];         //count of Bishops and Knights
    int materials[2];           //sum of piece values of each side
    UINT64_t pawns[3];          //representing white, black and all pawns on the board as 3 64-bit numbers
    int kings[2];
    int pieceList[13][10];      //easier to find each piece rather than going through pieces[] - - assuming the worst case scenario of having 10 of the same piece
//Most valuable victim Least valuable attacker scores array
    int MvvLvaScores[13][13]{};
    
    //board operating variables
    bool sideToMove;
    int enPassantSquares;
    int fiftyMoveCounter;
    int ply;                    //Number of half-moves played in the current search
    int totalPly;
    UINT64_t positionKey;
    int castlePermission;
    
    PVTable *pvTable = new PVTable();
private:
    static const int BitTable[64];
//score for each piece to calculte MvvLva scores
    const int victimScore[13] = {0, 100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600};
    //strings used for printing
    const std::string pieceCheck = ".PNBRQKpnbrqk";
    const std::string sideCheck = "wb-";
    const std::string rankCheck = "12345678";
    const std::string fileCheck = "ABCDEFGH";
    const int PvSize = 0x100000 * 2;
    
public:
    void initBoard();
    void initSquare120to64();
    void initBits();
    void initPositionKeys();
    void initFileRankArray();
    void InitEvalMasks();
    void initPvTable(PVTable *table);
    void initMvvLva();
    int PopBit(UINT64_t &Bitboard);
    int CountBits(UINT64_t Bitboard);
    void printBoard120_d();
    void printBoard64_d();
    void PrintBoard();
    void PrintBitBoard(UINT64_t BitBoard);
    std::string PrintSquare(const int square);
    UINT64_t randomNumber64();
    void resetBoard();
    void clearPvTable(PVTable *table);
//Authenticatinng methods
    bool isSquareOnBoard(const int square);
    bool isSideValid(const int side);
    bool isFileRankValid(const int FileRank);
    bool isPieceEmpty(const int piece);
    bool isPieceValid(const int piece);
};







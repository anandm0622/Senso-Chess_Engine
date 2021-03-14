//
//  BoardOperations.hpp
//  Senso
//
//  Created by Anand Manikandan on 2/28/21.
//

#pragma once

#include "BoardDefinitions.hpp"
#include <string>


#define MOVE(from, to, cap, prom, flag) ((from) | (to << 7) | (cap << 14) | (prom <<20) | (flag))
#define SquareOffboard(square) (squareToFile[square] == NOSQUARE)
//macros to update the position key
#define updatePiece(piece, square) (positionKey ^= (pieceKeys[piece][square]))
#define updateCastling (positionKey ^= (castleKeys[castlePermission]))
#define updateSide (positionKey ^= sideKey)
#define updateEnPassant (positionKey ^= pieceKeys[EMPTY][enPassantSquares])

class BoardOperations : public BoardDefinition {
public:
    //used to update pieceList
    static bool validateBigFive[13];
    static bool validateMajor[13];
    static bool validateMinor[13];
    static int validateValue[13];
    static int validateColor[13];
    
public:
    UINT64_t generatePositionKey();
    int parseFEN(std::string FEN);
    void updateMaterialsList();
    bool CheckBoard();
    void removePiece(const int square);
    void addPiece(const int piece, const int square);
};


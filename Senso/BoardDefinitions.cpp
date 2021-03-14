//
//  BoardDefinitions.cpp
//  Senso
//
//  Created by Anand Manikandan on 2/27/21.
//

#include "BoardDefinitions.hpp"
#include "Search.hpp"

//initialize static members
int BoardDefinition::orientation120to64[TotalSquares] = {};      //0 to 119 board
int BoardDefinition::orientation64to120[64] = {};                 //64 block board of 0 to 63 which is 21 to 98 on the 120-board
int BoardDefinition::squareToFile[TotalSquares] = {};
int BoardDefinition::squareToRank[TotalSquares] = {};
const int BoardDefinition::BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};
UINT64_t BoardDefinition::setBits[64] = {0ULL};
UINT64_t BoardDefinition::clearBits[64] = {0ULL};
UINT64_t BoardDefinition::BBRankMasks[8] = {0ULL};
UINT64_t BoardDefinition::BBFileMasks[8] = {0ULL};
UINT64_t BoardDefinition::whitePassedPawns[GameSquares] = {0ULL};
UINT64_t BoardDefinition::blackPassedPawns[GameSquares] = {0ULL};
UINT64_t BoardDefinition::isolatedPawns[GameSquares] = {0ULL};
UINT64_t BoardDefinition::pieceKeys[13][120] = {};                    //Key for every piece by the total squares on the board
UINT64_t BoardDefinition::sideKey = {};                               //Hash a unique key if it is White's turn to move
UINT64_t BoardDefinition::castleKeys[16] = {};                        //castling keys

void BoardDefinition::initBoard(){
    initSquare120to64();
    initBits();
    initPositionKeys();
    initFileRankArray();
    initMvvLva();
    initPvTable(pvTable);
    InitEvalMasks();
}

void BoardDefinition::initSquare120to64(){
    
    int file = FILE_A;
    int rank = RANK_1;
    int square = A1;
    int square64 = 0;
    
    for(int i = 0; i < TotalSquares; i++){                      //initialize all the 120 squares in the 120-squares board to "65"
       orientation120to64[i] = 65;
    }
    for(int i = 0; i < GameSquares; i++){                                //initialize all 64 squares in the 64-sqaure board to "120"
        orientation64to120[i] = 120;
    }
    for(rank = RANK_1; rank <= RANK_8; rank++){
        for(file = FILE_A; file <= FILE_H; file++){
            square = FileRankToSquare(file, rank);              //generate a number between 21 - 98 based on the file and rank
            orientation64to120[square64] = square;              //assign the generated number to the 64-square board, assigns 21 - 98 for 0 - 63
            orientation120to64[square] = square64;              //replaces all the squares except the border squares to the index values of the 64-square board
            square64++;
        }
    }
}

void BoardDefinition::initBits(){
    for(int i = 0; i < GameSquares; i++){
        setBits[i] |= (1ULL << i);
        clearBits[i] = ~setBits[i];
    }
}


void BoardDefinition::initPositionKeys(){
    for(int i = 0; i < array_size(pieceKeys); i++){
        for(int j = 0; j < array_size(pieceKeys[i]); j++){
            pieceKeys[i][j] = randomNumber64();
        }
    }
    sideKey = randomNumber64();
    for(int i = 0; i < array_size(castleKeys); i++){
        castleKeys[i] = randomNumber64();
    }
}

void BoardDefinition::initFileRankArray(){
    int square{};
    for(int i = 0; i < TotalSquares; i++){
        squareToFile[i] = NOSQUARE;
        squareToRank[i] = NOSQUARE;
    }
    for(int i = RANK_1; i <= RANK_8; i++){
        for(int j = FILE_A; j <= FILE_H; j++){
            square = FileRankToSquare(j, i);
            squareToFile[square] = j;
            squareToRank[square] = i;
        }
    }
}

void BoardDefinition::InitEvalMasks(){

    int rank{};
    int file{};
    int square{};
    int t_square{};

    for(rank = RANK_8; rank >= RANK_1; rank--){
        for (file = FILE_A; file <= FILE_H; file++){
            square = rank * 8 + file;
            BBFileMasks[file] |= (1ULL << square);
            BBRankMasks[rank] |= (1ULL << square);
        }
    }

    for(square = 0; square < 64; ++square){
        t_square = square + 8;

        while(t_square < 64) {
            whitePassedPawns[square] |= (1ULL << t_square);
            t_square += 8;
        }

        t_square = square - 8;
        while(t_square >= 0) {
            blackPassedPawns[square] |= (1ULL << t_square);
            t_square -= 8;
        }

        if(squareToFile[orientation64to120[square]] > FILE_A) {
            isolatedPawns[square] |= BBFileMasks[squareToFile[orientation64to120[square]] - 1];

            t_square = square + 7;
            while(t_square < 64) {
                whitePassedPawns[square] |= (1ULL << t_square);
                t_square += 8;
            }

            t_square = square - 9;
            while(t_square >= 0) {
                blackPassedPawns[square] |= (1ULL << t_square);
                t_square -= 8;
            }
        }

        if(squareToFile[orientation64to120[square]] < FILE_H) {
            isolatedPawns[square] |= BBFileMasks[squareToFile[orientation64to120[square]] + 1];

            t_square = square + 9;
            while(t_square < 64) {
                whitePassedPawns[square] |= (1ULL << t_square);
                t_square += 8;
            }

            t_square = square - 7;
            while(t_square >= 0) {
                blackPassedPawns[square] |= (1ULL << t_square);
                t_square -= 8;
            }
        }
    }
}


void BoardDefinition::initMvvLva(){
    for(int a = WhitePawn; a <= BlackKing; a++){
        for(int v = WhitePawn; v <= BlackKing; v++){
            MvvLvaScores[v][a] = victimScore[v] + 6 - (victimScore[a] / 100);
        }
    }
}

int BoardDefinition::PopBit(UINT64_t &Bitboard) {                                       //returns the first set index of the 64-square board and clears the bit
  UINT64_t tempBitboard = Bitboard ^ (Bitboard - 1);
  unsigned int fold = (unsigned) ((tempBitboard & 0xffffffff) ^ (tempBitboard >> 32));
  Bitboard &= (Bitboard - 1);
  return BitTable[(fold * 0x783a9b23) >> 26];
}

int BoardDefinition::CountBits(UINT64_t Bitboard) {                                 //returns the count of the number of set bits on the board using Hamming weight
    UINT64_t mask1 = 0x5555555555555555;
    UINT64_t mask2 = 0x3333333333333333;
    UINT64_t mask3 = 0x0F0F0F0F0F0F0F0F;
    UINT64_t mask4 = 0x00FF00FF00FF00FF;
    UINT64_t mask5 = 0x0000FFFF0000FFFF;
    UINT64_t mask6 = 0x00000000FFFFFFFF;
    
    Bitboard = (Bitboard & mask1) + ((Bitboard >> 1) & mask1);
    Bitboard = (Bitboard & mask2) + ((Bitboard >> 2) & mask2);
    Bitboard = (Bitboard & mask3) + ((Bitboard >> 4) & mask3);
    Bitboard = (Bitboard & mask4) + ((Bitboard >> 8) & mask4);
    Bitboard = (Bitboard & mask5) + ((Bitboard >> 16) & mask5);
    return static_cast<int>(((Bitboard & mask6) + ((Bitboard >> 32) & mask6)));
    
}

void BoardDefinition::printBoard120_d(){                            //prints the 120 square board
    for(int i = 0; i < TotalSquares; i++){
        if(i % 10 == 0)
            std::cout<<"\n";
        std::cout<<orientation120to64[i]<<"\t";
    }
    std::cout<<"\n\n";
}

void BoardDefinition::printBoard64_d(){                     //prints the 64 square board
    for(int i = 0; i < GameSquares; i++){
        if(i % 8 == 0)
            std::cout<<"\n";
        std::cout<<orientation64to120[i]<<"\t";
    }
    std::cout<<"\n\n";
}

void BoardDefinition::PrintBoard() {

    std::cout<<"\nCurrent Board:\n\n";

    for(int rank = RANK_8; rank >= RANK_1; rank--) {
        std::cout<<rank+1<<"\t";
        for(int file = FILE_A; file <= FILE_H; file++) {
            std::cout<<"\t"<<pieceCheck[pieces[FileRankToSquare(file,rank)]];
        }
        std::cout<<std::endl;
    }

    std::cout<<"\t";
    for(int file = FILE_A; file <= FILE_H; file++) {
        std::cout<<"\t"<<fileCheck[file];
    }
    std::cout<<std::endl;
    std::cout<<"side: "<<sideCheck[sideToMove]<<std::endl;
    std::cout<<"enPas: "<<enPassantSquares<<std::endl;
    std::cout<<"castle: "<< ((castlePermission & WhiteKingSideCastling) ? "K" : "-") <<
                            ((castlePermission & WhiteQueenSideCastling) ? "Q" :"-") <<
                            ((castlePermission & BlackKingSideCastling) ? "k" : "-") <<
                            ((castlePermission & BlackQueenSideCastling) ? "q" : "-") << std::endl;
    std::cout<<"PosKey: "<<positionKey<<std::endl;
}

void BoardDefinition::PrintBitBoard(UINT64_t BitBoard){                     //prints the current bitboard
    int rank{};
    int file{};
    int square{};
    int square64{};
    
    for(rank = RANK_8; rank >= RANK_1; rank--){
        for(file = FILE_A; file <= FILE_H; file++){
            square = FileRankToSquare(file, rank);  //120 board
            square64 = orientation120to64[square];  //64 board
            if((1ULL << square64) & BitBoard){
                std::cout<<"X";
            }else{
                std::cout<<"-";
            }
        }
        std::cout<<std::endl;
    }
    std::cout<<"\n\n";
}

std::string BoardDefinition::PrintSquare(const int square){

    std::string SquareStr;
    char tempStr[3] = {};
    int file = squareToFile[square];
    int rank = squareToRank[square];

    sprintf(tempStr, "%c%c", ('a'+ file), ('1'+ rank));
    SquareStr += tempStr[0];
    SquareStr += tempStr[1];
    return SquareStr;

}

UINT64_t BoardDefinition::randomNumber64(){                                                             //generate a 64-bit position key
    std::random_device random;
    std::mt19937_64 randomGenerator(random());                                                           //seed is the current time so the generation is always different
    std::uniform_int_distribution<unsigned long long> generate(0ULL, 18446744073709551615ULL);          // range is set to 0ULL to MaxULL
    return generate(randomGenerator);
}


void BoardDefinition::resetBoard(){
    for(int i = 0; i < TotalSquares; i++){                      //all the squares including the chess board are set to NOSQUARE
        pieces[i] = NOSQUARE;
    }
    for(int i = 0; i < GameSquares; i++){
        pieces[orientation64to120[i]] = EMPTY;                  //chess board squares are set to EMPTY denoting no pieces
    }
    for(int i = 0; i < 2; i++){                                 //re-initializing the piece's count to 0
        BigFivePieces[i] = 0;
        minorPieces[i] = 0;
        materials[i] = 0;
        majorPieces[i] = 0;
        pawns[i] = 0ULL;
    }
    kings[WHITE] = NOSQUARE;                                        //reseting the King's position
    kings[BLACK] = NOSQUARE;
    
    //reseting the side to move, en passant square and the fifty move counter, ply, total ply, castling permissions and position key.
    sideToMove = BOTH;
    enPassantSquares = NOSQUARE;
    fiftyMoveCounter = 0;
    ply = 0;
    totalPly = 0;
    castlePermission = 0;
    positionKey = 0ULL;
    
}

void BoardDefinition::initPvTable(PVTable *table){
    table->entries = PvSize/sizeof(PVEntry);
    table->entries -= 2;
    if(table->table != NULL){
        delete (table->table);
    }
    table->table = (PVEntry*) new PVEntry[table->entries * sizeof(PVEntry)]();
    clearPvTable(table);
    //std::cout<<"Principle Variation Table complete with "<<table->entries<<std::endl;
}

void BoardDefinition::clearPvTable(PVTable *table){
    for(PVEntry *entry = table->table; entry < (table->table + table->entries); entry++){
        entry->positionKey = 0ULL;
        entry->move = 0;
    }
}
//Authenticating methods

bool BoardDefinition::isSquareOnBoard(const int square){
    return (squareToFile[square] != NOSQUARE);
}
bool BoardDefinition::isSideValid(const int side){
    return (side == WHITE || side == BLACK);
}
bool BoardDefinition::isFileRankValid(const int FileRank){
    return (FileRank >= 0 && FileRank <= 7);
}
bool BoardDefinition::isPieceEmpty(const int piece){
    return (piece >= EMPTY && piece <= BlackKing);
}
bool BoardDefinition::isPieceValid(const int piece){
    return (piece >= WhitePawn && piece <= BlackKing);
}


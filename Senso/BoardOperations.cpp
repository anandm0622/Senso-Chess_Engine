//
//  BoardOperations.cpp
//  Senso
//
//  Created by Anand Manikandan on 2/28/21.
//

#include "BoardOperations.hpp"

//initializing static variables
bool BoardOperations::validateBigFive[13] = { false, false, true, true, true, true, true, false, true, true, true, true, true };
bool BoardOperations::validateMajor[13] = { false, false, false, false, true, true, true, false, false, false, true, true, true };
bool BoardOperations::validateMinor[13] = { false, false, true, true, false, false, false, false, true, true, false, false, false };
int BoardOperations::validateValue[13] = { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000  };
int BoardOperations::validateColor[13] = { BOTH, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

UINT64_t BoardOperations::generatePositionKey(){                        //generate a unique position key for every ply
    UINT64_t returnKey{};
    int piece = EMPTY;
    
    for(int square = 0; square < TotalSquares; square++){
        piece = pieces[square];
        if((piece != NOSQUARE) && (piece != EMPTY)){
            assert(piece >= WhitePawn && piece <= BlackKing);                 //sanity check
            returnKey ^= pieceKeys[piece][square];
        }
    }
    
    if(sideToMove == WHITE){
        returnKey ^= sideKey;
    }
    
    if(enPassantSquares != NOSQUARE){
        assert(enPassantSquares >= 0 && enPassantSquares < TotalSquares);     //sanity check
        returnKey ^= pieceKeys[EMPTY][enPassantSquares];
    }
        
    assert(castlePermission >= 0 && castlePermission <= 15);                    //sanity check
    return (returnKey ^= castleKeys[castlePermission]);
}


int BoardOperations::parseFEN(std::string FEN){                 //parses the FEN string
    assert(FEN.size() != 0);

    int  rank = RANK_8;
    int  file = FILE_A;
    int  piece{};
    int  count{};
    int  FENindex{};
    int  square64{};
    int  square120{};

    resetBoard();

    while ((rank >= RANK_1) && FEN[FENindex]) {         //traverses the FEN string
        count = 1;
        switch (FEN[FENindex]) {                                //assign the value to piece bases on the the FEN character
            case 'p': piece = BlackPawn; break;
            case 'r': piece = BlackRook; break;
            case 'n': piece = BlackKnight; break;
            case 'b': piece = BlackBishop; break;
            case 'k': piece = BlackKing; break;
            case 'q': piece = BlackQueen; break;
            case 'P': piece = WhitePawn; break;
            case 'R': piece = WhiteRook; break;
            case 'N': piece = WhiteKnight; break;
            case 'B': piece = WhiteBishop; break;
            case 'K': piece = WhiteKing; break;
            case 'Q': piece = WhiteQueen; break;

            case '1':                                               //if the read character is a digit count is increased
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                piece = EMPTY;
                count += FEN[FENindex] - '1';
                break;

            case '/':                                               //if '/' is read, it moves to the next rank so the rank is decreased and file is reset to FILE_A
            case ' ':
                rank--;
                file = FILE_A;
                FENindex++;
                continue;

            default:
                std::cout<<"FEN error \n";
                return -1;
        }
        //std::cout<<rank<<"\t"<<file<<"\t"<<count<<std::endl;
        for (int i = 0; i < count; i++) {                       //the piece is assigned to the square if the character read is a piece, else the square is moved based on the count.
            square64 = (rank * 8) + file;
            square120 = orientation64to120[square64];
            if (piece != EMPTY) {
                pieces[square120] = piece;
            }
            file++;
        }
        FENindex++;
    }
    
    //at the end of the loop, the entire board is read leaving the side to move, castling permissions and en passant square
    assert(FEN[FENindex] == 'w' || FEN[FENindex] == 'b');

    sideToMove = (FEN[FENindex] == 'w') ? WHITE : BLACK;            //side to move is read from FEN string and assigned
    FENindex += 2;

    for (int i = 0; i < 4; i++) {
        if (FEN[FENindex] == ' ') {
            break;
        }
        switch(FEN[FENindex]) {
            case 'K': castlePermission |= WhiteKingSideCastling; break;
            case 'Q': castlePermission |= WhiteQueenSideCastling; break;
            case 'k': castlePermission |= BlackKingSideCastling; break;
            case 'q': castlePermission |= BlackQueenSideCastling; break;
            default:         break;
        }
        FENindex++;
    }
    FENindex++;

    assert(castlePermission>=0 && castlePermission <= 15);

    if (FEN[FENindex] != '-') {
        file = FEN[FENindex] - 'a';
        rank = FEN[FENindex+1] - '1';
        
        assert(file>=FILE_A && file <= FILE_H);
        assert(rank>=RANK_1 && rank <= RANK_8);

        enPassantSquares = FileRankToSquare(file,rank);
    }
    updateMaterialsList();
    positionKey = generatePositionKey();    

    return 0;
}


void BoardOperations::updateMaterialsList(){
    int color;
    for(int i = 0; i < TotalSquares; i++){
        if(pieces[i] != NOSQUARE && pieces[i] != EMPTY){
            color = validateColor[pieces[i]];
            if(validateBigFive[pieces[i]])
                BigFivePieces[color]++;
            if(validateMajor[pieces[i]])
                majorPieces[color]++;
            if(validateMinor[pieces[i]])
                minorPieces[color]++;
            
            materials[color] += validateValue[pieces[i]];
            
            //updating piece list -> pieceList [piece][piece Number] ;
            pieceList[pieces[i]][pieceCount[pieces[i]]] = i; //pieceCount maintains the count for each piece and thus assigning the position of that piece on the 120-square board
            pieceCount[pieces[i]]++;
            
            //assigning King's position
            if(pieces[i] == WhiteKing)
                kings[WHITE] = i;
            if(pieces[i] == BlackKing)
                kings[BLACK] = i;
            
            //updating pawns
            if(pieces[i] == WhitePawn){
                setBit(pawns[WHITE], orientation120to64[i]);
                setBit(pawns[BOTH], orientation120to64[i]);
            }
            if(pieces[i] == BlackPawn){
                setBit(pawns[BLACK], orientation120to64[i]);
                setBit(pawns[BOTH], orientation120to64[i]);
            }
        }
    }
}


bool BoardOperations::CheckBoard() {
 
    int t_pieceCount[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int t_BigFivePieces[2] = { 0, 0};
    int t_majorPieces[2] = { 0, 0};
    int t_minorPieces[2] = { 0, 0};
    int t_materials[2] = { 0, 0};
    
    int square64, sq120, colour, pawncount, t_piece;
    
    UINT64_t t_pawns[3] = {0ULL, 0ULL, 0ULL};
    
    t_pawns[WHITE] = pawns[WHITE];
    t_pawns[BLACK] = pawns[BLACK];
    t_pawns[BOTH] = pawns[BOTH];
    
    // check piece lists
    for(t_piece = WhitePawn; t_piece <= BlackKing; ++t_piece) {
        for(int j = 0; j < pieceCount[t_piece]; ++j) {
            sq120 = pieceList[t_piece][j];
            assert(pieces[sq120] == t_piece);
        }
    }
    
    // check piece count and other counters
    for(square64 = 0; square64 < GameSquares; ++square64) {
        sq120 = orientation64to120[square64];
        t_piece = pieces[sq120];
        t_pieceCount[t_piece]++;
        colour = validateColor[t_piece];
        if( validateBigFive[t_piece] == true) t_BigFivePieces[colour]++;
        if( validateMinor[t_piece] == true) t_minorPieces[colour]++;
        if( validateMajor[t_piece] == true) t_majorPieces[colour]++;
        
        t_materials[colour] += validateValue[t_piece];
    }
    
    //check piece count
    for(t_piece = WhitePawn; t_piece <= BlackKing; ++t_piece) {
        assert(t_pieceCount[t_piece] == pieceCount[t_piece]);
    }
    
    // check bitboards count
    pawncount = CountBits(t_pawns[WHITE]);
    assert(pawncount == pieceCount[WhitePawn]);
    pawncount = CountBits(t_pawns[BLACK]);
    assert(pawncount == pieceCount[BlackPawn]);
    pawncount = CountBits(t_pawns[BOTH]);
    assert(pawncount == (pieceCount[BlackPawn] + pieceCount[WhitePawn]));
    
    // check bitboards squares
    while(t_pawns[WHITE]) {
        square64 = PopBit(t_pawns[WHITE]);
        assert(pieces[orientation64to120[square64]] == WhitePawn);
    }
    while(t_pawns[BLACK]) {
        square64 = PopBit(t_pawns[BLACK]);
        assert(pieces[orientation64to120[square64]] == BlackPawn);
    }
    while(t_pawns[BOTH]) {
        square64 = PopBit(t_pawns[BOTH]);
        assert(pieces[orientation64to120[square64]] == WhitePawn ||  pieces[orientation64to120[square64]] == BlackPawn);
    }
    
    assert(t_materials[WHITE] == materials[WHITE] && t_materials[BLACK] == materials[BLACK]);
    assert(t_minorPieces[WHITE] == minorPieces[WHITE] && t_minorPieces[BLACK] == minorPieces[BLACK]);
    assert(t_majorPieces[WHITE] == majorPieces[WHITE] && t_majorPieces[BLACK] == majorPieces[BLACK]);
    assert(t_BigFivePieces[WHITE] == BigFivePieces[WHITE] && t_BigFivePieces[BLACK] == BigFivePieces[BLACK]);
    
    assert(sideToMove == WHITE || sideToMove == BLACK);
    assert(generatePositionKey() == positionKey);
    
    assert(enPassantSquares == NOSQUARE || ( squareToRank[enPassantSquares] == RANK_6 && sideToMove == WHITE) || (squareToRank[enPassantSquares] == RANK_3 && sideToMove == BLACK));
    
    assert(pieces[kings[WHITE]] == WhiteKing);
    assert(pieces[kings[BLACK]] == BlackKing);
         
    return true;
}

void BoardOperations::removePiece(const int square){
//verifying the sqaure and the piece on that sqaure
    assert(isSquareOnBoard(square));
    int piece = pieces[square];
    assert(isPieceValid(piece));
    
//getting the color of the piece to be removed
    int pieceColor = validateColor[piece];
    int t_pieceCount = -1;
    assert(isSideValid(pieceColor));
//updating the positionKey
    updatePiece(piece, square);
//updating the board
    pieces[square] = EMPTY;
    materials[pieceColor] -= validateValue[piece];
//updating the piece count
    if(validateBigFive[piece]){
        BigFivePieces[pieceColor]--;
        if(validateMajor[piece]){
            majorPieces[pieceColor]--;
        }else{
            minorPieces[pieceColor]--;
        }
//if it isn't a makor or a minor piece it is considered as a pawn and the pawn bitboard is updated
    }else{
        clearBit(pawns[pieceColor], orientation120to64[square]);
        clearBit(pawns[BOTH], orientation120to64[square]);
    }
//updating the pieceList
//getting the index of the piece from the pieceList
    for(int i = 0; i < pieceCount[piece]; i++){
        if(pieceList[piece][i] == square){
            t_pieceCount = i;
            break;
        }
    }
    assert(t_pieceCount != -1);
    assert(t_pieceCount >= 0 && t_pieceCount < 10);
//reducing the total count of the piece removed
    pieceCount[piece]--;
/*
replacing the position of the current piece by the position of the last piece
this will reduce the array size and update the array to correspond only to the current pieces on board
*/
    pieceList[piece][t_pieceCount] = pieceList[piece][pieceCount[piece]];
}

void BoardOperations::addPiece(const int piece, const int square){
//verifying the piece and sqaure
    assert(isPieceValid(piece));
    assert(isSquareOnBoard(square));
//getting the piece color
    int pieceColor = validateColor[piece];
    assert(isSideValid(pieceColor));
//update the position key
    updatePiece(piece, square);
    pieces[square] = piece;
//updating the piece count
    if(validateBigFive[piece]){
        BigFivePieces[pieceColor]++;
        if(validateMajor[piece]){
            majorPieces[pieceColor]++;
        }else{
            minorPieces[pieceColor]++;
        }
//if it isn't a makor or a minor piece it is considered as a pawn and the pawn bitboard is updated
    }else{
        setBit(pawns[pieceColor], orientation120to64[square]);
        setBit(pawns[BOTH], orientation120to64[square]);
    }
//updating the board
    materials[pieceColor] += validateValue[piece];
//update to the pieceList and increment the count
    pieceList[piece][pieceCount[piece]++] = square;
}

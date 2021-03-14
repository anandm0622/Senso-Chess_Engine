//
//  BoardMoves.cpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/2/21.
//

#include "BoardMoves.hpp"

//initializing static data members
bool BoardMoves::attackingSquarePawn[13] = { false, true, false, false, false, false, false, true, false, false, false, false, false };
bool BoardMoves::attackingSquareKnight[13] = { false, false, true, false, false, false, false, false, true, false, false, false, false };
bool BoardMoves::attackingSquareKing[13] = { false, false, false, false, false, false, true, false, false, false, false, false, true };
bool BoardMoves::attackingSquareRook[13] = { false, false, false, false, true, true, false, false, false, false, true, true, false };                   //for both queen and rook
bool BoardMoves::attackingSquareBishop[13] = { false, false, false, true, false, true, false, false, false, true, false, true, false };                 //for both bishop and rook
bool BoardMoves::PieceSlides[13] = { false, false, false, true, true, true, false, false, false, true, true, true, false };

std::string BoardMoves::PrintMove(const int move) {

    std::string MoveStr;
    char tempStr[6] = {};
    int ff = squareToFile[getFromSquare(move)];
    int rf = squareToRank[getFromSquare(move)];
    int ft = squareToFile[getToSquare(move)];
    int rt = squareToRank[getToSquare(move)];

    int promoted = getPromotedPiece(move);

    if(promoted) {
        char pchar = 'q';
        if(attackingSquareKnight[promoted]) {
            pchar = 'n';
        } else if(attackingSquareRook[promoted] && !attackingSquareBishop[promoted])  {
            pchar = 'r';
        } else if(!attackingSquareRook[promoted] && attackingSquareBishop[promoted])  {
            pchar = 'b';
        }
        sprintf(tempStr, "%c%c%c%c%c", ('a'+ff),('1'+rf), ('a'+ft), ('1'+rt), pchar);
    } else {
        sprintf(tempStr, "%c%c%c%c", ('a'+ff),('1'+rf), ('a'+ft), ('1'+rt));
    }
    for(int i = 0; i < array_size(tempStr); i++){
        MoveStr += tempStr[i];
    }
    return MoveStr;
}
/*----------------------------------------------Move Generation--------------------------------------------------*/

bool BoardMoves::attackingSquare(const int square, const int attackingSide){
    int t_piece;
    int t_square;
    
    //sanity checks
    assert(isSquareOnBoard(square));
    assert(isSideValid(attackingSide));
    assert(CheckBoard());
    
    //is a pawn attacking
    if(attackingSide == WHITE){
        if(pieces[square - 11] == WhitePawn || pieces[square - 9] == WhitePawn)
            return true;
    }else{
        if(pieces[square + 11] == BlackPawn || pieces[square + 9] == BlackPawn)
            return true;
    }
    //is a knight attacking
    for(int i = 0; i < array_size(attackingPieceKnight); i++){
        t_piece = pieces[square + attackingPieceKnight[i]];
        if(attackingSquareKnight[t_piece] && validateColor[t_piece] == attackingSide && t_piece != NOSQUARE)
            return true;
    }
    
    //is a rook or queen attacking
    for(int i = 0; i < array_size(attackingPieceRook); i++){
        t_square = attackingPieceRook[i] + square;
        t_piece = pieces[t_square];
        while(t_piece != NOSQUARE){
            if(t_piece != EMPTY){
                if(attackingSquareRook[t_piece] && validateColor[t_piece] == attackingSide)
                    return true;
                break;
            }
            t_square += attackingPieceRook[i];
            t_piece = pieces[t_square];
        }
    }
    //is a bishop or a queen attacking
    for(int i = 0; i < array_size(attackingPieceBishop); i++){
        t_square = attackingPieceBishop[i] + square;
        t_piece = pieces[t_square];
        while(t_piece != NOSQUARE){
            if(t_piece != EMPTY){
                if(attackingSquareBishop[t_piece] && validateColor[t_piece] == attackingSide)
                    return true;
                break;
            }
            t_square += attackingPieceBishop[i];
            t_piece = pieces[t_square];
        }
    }
    //is a King attacking
    for(int i = 0; i < array_size(attackingPieceKing); i++){
        t_piece = pieces[square + attackingPieceKing[i]];
        if(attackingSquareKing[t_piece] && validateColor[t_piece] == attackingSide && t_piece != NOSQUARE)
            return true;
    }
    
    return false;
}

//checks if there are any repeated moves
bool BoardMoves::isRepeated(){
    for(int i = (totalPly - fiftyMoveCounter); i < (totalPly - 1); i++){
        assert(i >= 0 && i < MaximumGameMoves);
        if(positionKey == history[i].positionKey)
            return true;
    }
    return false;
}

void BoardMoves::PrintMoveList(){
    std::cout<<"Move List Number: "<<moveList.count<<std::endl;
    for(int i = 0; i < moveList.count; i++){
        std::cout<<"Move: "<<(i+1)<<" -> "<<PrintMove(moveList.moves[i].move)<<" Score: "<<moveList.moves[i].score<<std::endl;
    }
    std::cout<<std::endl;
}

int BoardMoves::parseMove(std::string move){
    if(move[1] > '8' || move[1] < '1') return false;
    if(move[3] > '8' || move[3] < '1') return false;
    if(move[0] > 'h' || move[0] < 'a') return false;
    if(move[0] > 'h' || move[2] < 'a') return false;
    
    int from = FileRankToSquare((move[0] - 'a'), (move[1] - '1'));
    int to = FileRankToSquare((move[2] - 'a'), (move[3] - '1'));
    int returnMove{};
    int promoted{};
    
    assert(isSquareOnBoard(from) && isSquareOnBoard(to));
    MoveList *list = new MoveList();
    GenerateMoves(list);
    for(int i = 0; i < list->count; i++){
        returnMove = list->moves[i].move;
        if(getFromSquare(returnMove) == from && getToSquare(returnMove) == to){
            promoted = getPromotedPiece(returnMove);
            if(promoted != EMPTY){
                if((attackingSquareRook[promoted]) && (!attackingSquareBishop[promoted]) && (move[4] == 'r')){
                    return returnMove;
                }else if(!attackingSquareRook[promoted] && attackingSquareBishop[promoted] && (move[4] == 'b')){
                    return returnMove;
                }else if(attackingSquareRook[promoted] && attackingSquareBishop[promoted] && (move[4] == 'q')){
                    return returnMove;
                }else if(attackingSquareKnight[promoted] && (move[4] == 'n')){
                    return returnMove;
                }
                continue;
            }
            return returnMove;
        }
    }
    return false;
}

void BoardMoves::AddQuietMove(int move, MoveList* moveList) {

    moveList->moves[moveList->count].move = move;
    if(searchKillers[0][ply] == move){
        moveList->moves[moveList->count].score = 900000;
    }else if(searchKillers[1][ply] == move){
        moveList->moves[moveList->count].score = 800000;
    }else{
        moveList->moves[moveList->count].score = searchHistory[pieces[getFromSquare(move)]][getToSquare(move)];
    }
    moveList->count++;
}

void BoardMoves::AddCaptureMove(int move, MoveList* moveList) {

    moveList->moves[moveList->count].move = move;
    moveList->moves[moveList->count].score = MvvLvaScores[getCapturedPiece(move)][pieces[getFromSquare(move)]] + 1000000;
    moveList->count++;
}

void BoardMoves::AddEnPassantMove(int move, MoveList* moveList) {

    moveList->moves[moveList->count].move = move;
    moveList->moves[moveList->count].score = 105 + 1000000;
    moveList->count++;
}

void BoardMoves::AddPawnMove(const int from, const int to, const int cap, MoveList* moveList) {

    assert(isSquareOnBoard(from));
    assert(isSquareOnBoard(to));
    if(cap != EMPTY)
        assert(isPieceValid(cap));
    if(sideToMove == WHITE){
        if(cap != EMPTY){
            if(squareToRank[from] == RANK_7) {
                for(int i = WhiteKnight; i <= WhiteQueen; i++){
                    AddCaptureMove(MOVE(from,to,cap,i,0), moveList);
                }
            }else{
                AddCaptureMove(MOVE(from,to,cap,EMPTY,0), moveList);
            }
        }else{
            if(squareToRank[from] == RANK_7) {
                for(int i = WhiteKnight; i <= WhiteQueen; i++){
                    AddQuietMove(MOVE(from,to,EMPTY,i,0), moveList);
                }
            }else{
                AddQuietMove(MOVE(from,to,EMPTY,EMPTY,0), moveList);
            }
        }
    }else{
        if(cap != EMPTY){
            if(squareToRank[from] == RANK_2) {
                for(int i = BlackKnight; i <= BlackQueen; i++){
                    AddCaptureMove(MOVE(from,to,cap,i,0), moveList);
                }
            }else{
                AddCaptureMove(MOVE(from,to,cap,EMPTY,0), moveList);
            }
        }else{
            if(squareToRank[from] == RANK_2) {
                for(int i = BlackKnight; i <= BlackQueen; i++){
                    AddQuietMove(MOVE(from,to,EMPTY,i,0), moveList);
                }
            }else{
                AddQuietMove(MOVE(from,to,EMPTY,EMPTY,0), moveList);
            }
        }
    }
}

void BoardMoves::GenerateMoves(MoveList* moveList){
    assert(CheckBoard());
    moveList->count = 0;
    int t_piece = EMPTY;
    int t_pieceIndex{};
    int t_square{};
    int t_index{};
    int tempSquare{};
//variables to iterate through pawn moves, used in order to avoid repeatition in code
    int pawnMoves[8] = {10, 20, 9, 11, -10, -20, -9, -11};
    int pawnIndex{};
    int pawn{};
    int pawnRank{};
    
    if(sideToMove == WHITE){
//set White pawn index
        pawnIndex = 0;
        pawn = WhitePawn;
        pawnRank = RANK_2;
//checking for castling permissions
        if(castlePermission & WhiteKingSideCastling){
//checking whether the square the king and rook will move after castling are EMPTY and not being attacked and the King is not in 'Check'
            if(pieces[F1] == EMPTY && pieces[G1] == EMPTY){
                if((!attackingSquare(E1, BLACK)) && (!attackingSquare(F1, BLACK))){
                    AddQuietMove(MOVE(E1, G1, EMPTY, EMPTY, flagCastliing), moveList);
                }
            }
        }
        if(castlePermission & WhiteQueenSideCastling){
//checking whether the square the king and rook will move after castling are EMPTY and not being attacked and the King is not in 'Check'
            if(pieces[C1] == EMPTY && pieces[D1] == EMPTY && pieces[B1] == EMPTY){
                if((!attackingSquare(E1, BLACK)) && (!attackingSquare(D1, BLACK))){
                    AddQuietMove(MOVE(E1, C1, EMPTY, EMPTY, flagCastliing), moveList);
                }
            }
        }
    }else{
//set Black pawn Index
        pawnIndex = 4;
        pawn = BlackPawn;
        pawnRank = RANK_7;
//checking for castling permissions
        if(castlePermission & BlackKingSideCastling){
//checking whether the square the king and rook will move after castling are EMPTY and not being attacked and the King is not in 'Check'
            if(pieces[F8] == EMPTY && pieces[G8] == EMPTY){
                if((!attackingSquare(E8, WHITE)) && (!attackingSquare(F8, WHITE))){
                    AddQuietMove(MOVE(E8, G8, EMPTY, EMPTY, flagCastliing), moveList);
                }
            }
        }
        if(castlePermission & BlackQueenSideCastling){
//checking whether the square the king and rook will move after castling are EMPTY and not being attacked and the King is not in 'Check'
            if(pieces[C8] == EMPTY && pieces[D8] == EMPTY && pieces[B8] == EMPTY){
                if((!attackingSquare(E8, WHITE)) && (!attackingSquare(D8, WHITE))){
                    AddQuietMove(MOVE(E8, C8, EMPTY, EMPTY, flagCastliing), moveList);
                }
            }
        }
    }
    //generating pawn moves
    for(int i = 0; i < pieceCount[pawn]; i++){
        t_square = pieceList[pawn][i];
//checking whether the sqaure the piece is on is actually on the game board
        assert(isSquareOnBoard(t_square));
//checking whether the square in front is empty, if the pawn is at RANK_7 then getting promoted is taken care by AddWhitePawnMove()
        if(pieces[t_square + pawnMoves[pawnIndex]] == EMPTY){
            AddPawnMove(t_square, (t_square+pawnMoves[pawnIndex]), EMPTY, moveList);
//if it is at RANK_2 then we're checking whether the square two spaces ahead is empty, if it is then we set the flagPawnStart
            if(pieces[t_square + pawnMoves[pawnIndex+1]] == EMPTY && squareToRank[t_square] == pawnRank)
                AddQuietMove(MOVE(t_square, (t_square + pawnMoves[pawnIndex+1]), EMPTY, EMPTY, flagPawnStart), moveList);
        }
/*
checking whether the two capture squares for white pawns (i.e. current square + 9 and current square + 11 are occupied by any black pieces,
if yes, then call AddWhitePawnCapture and record the move
*/
        if(!SquareOffboard(t_square + pawnMoves[pawnIndex+2]) && validateColor[pieces[t_square + pawnMoves[pawnIndex+2]]] == (sideToMove ^ 1)){
            AddPawnMove(t_square, (t_square + pawnMoves[pawnIndex+2]), pieces[t_square + pawnMoves[pawnIndex+2]], moveList);
        }
        if(!SquareOffboard(t_square + pawnMoves[pawnIndex+3]) && validateColor[pieces[t_square + pawnMoves[pawnIndex+3]]] == (sideToMove ^ 1)){
            AddPawnMove(t_square, (t_square + pawnMoves[pawnIndex+3]), pieces[t_square + pawnMoves[pawnIndex+3]], moveList);
        }
//checking whether there are any en passant captures available, if yes call AddCaptureMove with en passant flag set
        if(enPassantSquares != NOSQUARE){
            if((t_square + pawnMoves[pawnIndex+2]) == enPassantSquares){
                AddEnPassantMove(MOVE(t_square, (t_square + pawnMoves[pawnIndex+2]), EMPTY, EMPTY, flagEnPassant), moveList);
            }
            if((t_square + pawnMoves[pawnIndex+3]) == enPassantSquares){
                AddEnPassantMove(MOVE(t_square, (t_square + pawnMoves[pawnIndex+3]), EMPTY, EMPTY, flagEnPassant), moveList);
            }
        }
    }

/*loop through sliding pieces and iterating through the possible squares they can land on
*/
    t_pieceIndex = slidingPieceIndex[sideToMove];
    t_piece = slidingPieces[t_pieceIndex++];
    while(t_piece != 0){
        assert(isPieceValid(t_piece));
        for(int i = 0; i < pieceCount[t_piece]; i++){
            t_square = pieceList[t_piece][i];
            assert(isSquareOnBoard(t_square));
            for(int j = 0; j < totalSquares[t_piece]; j++){
                t_index = availableSquares[t_piece][j];
                tempSquare = t_square + t_index;
/*if they land on an EMPTY square implement a quite move else initiate a capture
as sliding pieces have multiple possible sqaures in every direction, iterate through all of those untii you run off board.
*/
                while(!SquareOffboard(tempSquare)){
                    if(pieces[tempSquare] != EMPTY){
                        if(validateColor[pieces[tempSquare]] == (sideToMove ^ 1)){
                            AddCaptureMove(MOVE(t_square, tempSquare, pieces[tempSquare], EMPTY, 0), moveList);
                        }
                        break;
                    }
                    AddQuietMove(MOVE(t_square, tempSquare, EMPTY, EMPTY, 0), moveList);
                    tempSquare += t_index;
                }
            }
        }
        t_piece = slidingPieces[t_pieceIndex++];
    }
/*loop through sliding pieces and iterating through the possible squares they can land on
*/
    t_pieceIndex = nonSlidingPieceIndex[sideToMove];
    t_piece = nonSlidingPieces[t_pieceIndex++];
    while(t_piece != 0){
        assert(isPieceValid(t_piece));
        for(int i = 0; i < pieceCount[t_piece]; i++){
            t_square = pieceList[t_piece][i];
            assert(isSquareOnBoard(t_square));
            for(int j = 0; j < totalSquares[t_piece]; j++){
                t_index = availableSquares[t_piece][j];
                tempSquare = t_square + t_index;
//if they land on an EMPTY square implement a quite move else initiate a capture, if the sqaure is empty move to the next possible square
                if(SquareOffboard(tempSquare))
                    continue;
                if(pieces[tempSquare] != EMPTY){
                    if(validateColor[pieces[tempSquare]] == (sideToMove ^ 1)){
                        AddCaptureMove(MOVE(t_square, tempSquare, pieces[tempSquare], EMPTY, 0), moveList);
                    }
                    continue;
                }
                AddQuietMove(MOVE(t_square, tempSquare, EMPTY, EMPTY, 0), moveList);
            }
        }
        t_piece = nonSlidingPieces[t_pieceIndex++];
    }
}


void BoardMoves::GenerateCaptureMoves(MoveList* moveList){
    assert(CheckBoard());
    moveList->count = 0;
    int t_piece = EMPTY;
    int t_pieceIndex{};
    int t_square{};
    int t_index{};
    int tempSquare{};
//variables to iterate through pawn moves, used in order to avoid repeatition in code
    int pawnMoves[8] = {10, 20, 9, 11, -10, -20, -9, -11};
    int pawnIndex{};
    int pawn{};
    int pawnRank{};
    
    if(sideToMove == WHITE){
//set White pawn index
        pawnIndex = 0;
        pawn = WhitePawn;
        pawnRank = RANK_2;
    }else{
//set Black pawn Index
        pawnIndex = 4;
        pawn = BlackPawn;
        pawnRank = RANK_7;
    }
    //generating pawn moves
    for(int i = 0; i < pieceCount[pawn]; i++){
        t_square = pieceList[pawn][i];
//checking whether the sqaure the piece is on is actually on the game board
        assert(isSquareOnBoard(t_square));
/*
checking whether the two capture squares for white pawns (i.e. current square + 9 and current square + 11 are occupied by any black pieces,
if yes, then call AddWhitePawnCapture and record the move
*/
        if(!SquareOffboard(t_square + pawnMoves[pawnIndex+2]) && validateColor[pieces[t_square + pawnMoves[pawnIndex+2]]] == (sideToMove ^ 1)){
            AddPawnMove(t_square, (t_square + pawnMoves[pawnIndex+2]), pieces[t_square + pawnMoves[pawnIndex+2]], moveList);
        }
        if(!SquareOffboard(t_square + pawnMoves[pawnIndex+3]) && validateColor[pieces[t_square + pawnMoves[pawnIndex+3]]] == (sideToMove ^ 1)){
            AddPawnMove(t_square, (t_square + pawnMoves[pawnIndex+3]), pieces[t_square + pawnMoves[pawnIndex+3]], moveList);
        }
//checking whether there are any en passant captures available, if yes call AddCaptureMove with en passant flag set
        if(enPassantSquares != NOSQUARE){
            if((t_square + pawnMoves[pawnIndex+2]) == enPassantSquares){
                AddEnPassantMove(MOVE(t_square, (t_square + pawnMoves[pawnIndex+2]), EMPTY, EMPTY, flagEnPassant), moveList);
            }
            if((t_square + pawnMoves[pawnIndex+3]) == enPassantSquares){
                AddEnPassantMove(MOVE(t_square, (t_square + pawnMoves[pawnIndex+3]), EMPTY, EMPTY, flagEnPassant), moveList);
            }
        }
    }

/*loop through sliding pieces and iterating through the possible squares they can land on
*/
    t_pieceIndex = slidingPieceIndex[sideToMove];
    t_piece = slidingPieces[t_pieceIndex++];
    while(t_piece != 0){
        assert(isPieceValid(t_piece));
        for(int i = 0; i < pieceCount[t_piece]; i++){
            t_square = pieceList[t_piece][i];
            assert(isSquareOnBoard(t_square));
            for(int j = 0; j < totalSquares[t_piece]; j++){
                t_index = availableSquares[t_piece][j];
                tempSquare = t_square + t_index;
/*if they land on an EMPTY square implement a quite move else initiate a capture
as sliding pieces have multiple possible sqaures in every direction, iterate through all of those untii you run off board.
*/
                while(!SquareOffboard(tempSquare)){
                    if(pieces[tempSquare] != EMPTY){
                        if(validateColor[pieces[tempSquare]] == (sideToMove ^ 1)){
                            AddCaptureMove(MOVE(t_square, tempSquare, pieces[tempSquare], EMPTY, 0), moveList);
                        }
                        break;
                    }
                    tempSquare += t_index;
                }
            }
        }
        t_piece = slidingPieces[t_pieceIndex++];
    }
/*loop through sliding pieces and iterating through the possible squares they can land on
*/
    t_pieceIndex = nonSlidingPieceIndex[sideToMove];
    t_piece = nonSlidingPieces[t_pieceIndex++];
    while(t_piece != 0){
        assert(isPieceValid(t_piece));
        for(int i = 0; i < pieceCount[t_piece]; i++){
            t_square = pieceList[t_piece][i];
            assert(isSquareOnBoard(t_square));
            for(int j = 0; j < totalSquares[t_piece]; j++){
                t_index = availableSquares[t_piece][j];
                tempSquare = t_square + t_index;
//if they land on an EMPTY square implement a quite move else initiate a capture, if the sqaure is empty move to the next possible square
                if(SquareOffboard(tempSquare))
                    continue;
                if(pieces[tempSquare] != EMPTY){
                    if(validateColor[pieces[tempSquare]] == (sideToMove ^ 1)){
                        AddCaptureMove(MOVE(t_square, tempSquare, pieces[tempSquare], EMPTY, 0), moveList);
                    }
                    continue;
                }
            }
        }
        t_piece = nonSlidingPieces[t_pieceIndex++];
    }
}
/*----------------------------------------------Making a Move--------------------------------------------------*/

void BoardMoves::movePiece(const int from, const int to){
    assert(isSquareOnBoard(from));
    assert(isSquareOnBoard(to));
    
    int t_piece = pieces[from];
    int pieceColor = validateColor[t_piece];
    int t_pieceCount = false;
//update the positionKey
    updatePiece(t_piece, from);
    pieces[from] = EMPTY;
    updatePiece(t_piece, to);
    pieces[to] = t_piece;
    
//if the piece a pawn, update the pawnbitboard
    if(!validateBigFive[t_piece]){
        clearBit(pawns[pieceColor], orientation120to64[from]);
        clearBit(pawns[BOTH], orientation120to64[from]);
        setBit(pawns[pieceColor], orientation120to64[to]);
        setBit(pawns[BOTH], orientation120to64[to]);
    }
//update the pieceList
    for(int i = 0; i < pieceCount[t_piece]; i++){
        if(pieceList[t_piece][i] == from){
            pieceList[t_piece][i] = to;
            t_pieceCount = true;
            break;
        }
    }
    assert(t_pieceCount);
}

bool BoardMoves::makeMove(int move){
    assert(CheckBoard());
    int from = getFromSquare(move);
    int to = getToSquare(move);
//check whether the move is valid
    assert(isSquareOnBoard(from));
    assert(isSquareOnBoard(to));
    assert(isSideValid(sideToMove));
    assert(isPieceValid(pieces[from]));
//update history
    history[totalPly].positionKey = positionKey;
    
//remove pawns if the move made is an en passant capture
    if(move & flagEnPassant){
        if(sideToMove == WHITE){
            removePiece((to - 10));
        }else{
            removePiece((to + 10));
        }
//if it is a castling move, then the rook has to be additionally moved
    }else if(move & flagCastliing){
        switch (to) {
            case C1: movePiece(A1, D1); break;
            case C8: movePiece(A8, D8); break;
            case G1: movePiece(H1, F1); break;
            case G8: movePiece(H8, F8); break;
            default: assert(false); break;
        }
    }
//removing the castling permission by XORing to the position key and same with the  en passant sqaure
    if(enPassantSquares != NOSQUARE)
        updateEnPassant;
    updateCastling;
    
//add move to the History
    history[totalPly].lastMove = move;
    history[totalPly].fiftyMoveCounter = fiftyMoveCounter;
    history[totalPly].enPassantSquare = enPassantSquares;
    history[totalPly].castlePermission = castlePermission;
//updating the castling permission after the move is been made and adding that to the position key
    castlePermission &= CastlingBits[from];
    castlePermission &= CastlingBits[to];
//make en passant to EMPTY as the move is been made
    enPassantSquares = NOSQUARE;
    updateCastling;
//if this is a capture move, remove the piece and reset the fiftyMoveCounter
    int captured = getCapturedPiece(move);
    fiftyMoveCounter++;
    
    if(captured != EMPTY){
        assert(isPieceValid(captured));
        removePiece(to);
        fiftyMoveCounter = 0;
    }
//update the half moves made
    totalPly++;
    ply++;
//check and update if the move made is creating an en passant square on the board
    if(attackingSquarePawn[pieces[from]]){
        fiftyMoveCounter = 0;
        if(move & flagPawnStart){
            if(sideToMove == WHITE){
                enPassantSquares = (from + 10);
                assert(squareToRank[enPassantSquares] == RANK_3);
            }else{
                enPassantSquares = (from - 10);
                assert(squareToRank[enPassantSquares] == RANK_6);
            }
            updateEnPassant;
        }
    }
//as the captured pieces have been removed and flags have been set, make the move
    movePiece(from, to);
    
//check if a pawn has been promoted
    int promoted = getPromotedPiece(move);
    if(promoted != EMPTY){
        assert(isPieceValid(promoted) && !attackingSquarePawn[promoted]);
        removePiece(to);
        addPiece(promoted, to);
    }
//if the moved piece is a king update Kings[]
    if(attackingSquareKing[pieces[to]]){
        kings[sideToMove] = to;
    }
//update sideToMove
    sideToMove ^= 1;
    updateSide;
    assert(CheckBoard());
    
//check if the move made puts the king of teh same side on check, if so, then the move cannot be made
    if(attackingSquare(kings[sideToMove^1], sideToMove)){
        undoMove();
        return false;
    }
    return true;
}

void BoardMoves::undoMove(){
    assert(CheckBoard());
    totalPly--;
    ply--;
    
    int move = history[totalPly].lastMove;
    int from = getFromSquare(move);
    int to = getToSquare(move);
    assert(isSquareOnBoard(from));
    assert(isSquareOnBoard(to));
//by XORing the en passant squares and castling permission you remove it from the position key
    if(enPassantSquares != NOSQUARE)
        updateEnPassant;
    updateCastling;
//update the values from history and XOR the en passant and castle permissions
    castlePermission = history[totalPly].castlePermission;
    fiftyMoveCounter = history[totalPly].fiftyMoveCounter;
    enPassantSquares = history[totalPly].enPassantSquare;
    if(enPassantSquares != NOSQUARE)
        updateEnPassant;
    updateCastling;
//update sideToMove
    sideToMove ^= 1;
    updateSide;
//un-doing en passant and castling moves
    if(move & flagEnPassant){
        if(sideToMove == WHITE){
            addPiece(BlackPawn, (to - 10));
        }else{
            addPiece(WhitePawn, (to + 10));
        }
    }else if(move & flagCastliing){
        switch (to) {
            case C1: movePiece(D1, A1); break;
            case C8: movePiece(D8, A8); break;
            case G1: movePiece(F1, H1); break;
            case G8: movePiece(F8, H8); break;
            default: assert(false); break;
        }
    }
//move the piece back before replacing the captured piece, passing the arguments in reverse
//piece moved from 'to' square to 'from' square
    movePiece(to, from);
    if(attackingSquareKing[pieces[from]]){
        kings[sideToMove] = from;
    }
    int captured = getCapturedPiece(move);
    if(captured != EMPTY){
        assert(isPieceValid(captured));
        addPiece(captured, to);
    }
//remove the promoted piece if any and add pawn to the from square
    int promoted = getPromotedPiece(move);
    int t_pawn = BlackPawn;
    if(sideToMove == WHITE)
        t_pawn = WhitePawn;
    if(promoted != EMPTY){
        assert(isPieceValid(promoted));
        removePiece(from);
        addPiece(t_pawn, from);
    }
    
    assert(CheckBoard());
}

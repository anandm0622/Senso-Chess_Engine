//
//  PerformanceTest.cpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/7/21.
//

#include "Search.hpp"

Search:: ~Search(){
    delete info;
}

/*----------------------------------------------Performance Testing Implementation--------------------------------------------------*/

void Search::perfTesting(int depth){
    assert(CheckBoard());
    if(depth == 0){
        nodes++;
        return;
    }
    MoveList *list = new MoveList();
    GenerateMoves(list);
    for(int i = 0; i < list->count; i++){
        if(!makeMove(list->moves[i].move)){
            continue;
        }
        perfTesting(depth-1);
        undoMove();
    }
    delete list;
    return;
}

void Search::runTest(int depth){
    assert(CheckBoard());
    PrintBoard();
    std::cout<<"Starting test to depth: "<<depth<<std::endl;
    auto start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    nodes = 0;
    MoveList *t_list = new MoveList();
    GenerateMoves(t_list);
    
    int move{};
    for(int i = 0; i < t_list->count; i++){
        move = t_list->moves[i].move;
        if(!makeMove(move)){
            continue;
        }
        long long leafNodes = nodes;
        perfTesting(depth - 1);
        undoMove();
        long long parentNodes = nodes - leafNodes;
        std::cout<<"Move "<<i+1<<": "<<PrintMove(move)<<" : "<<parentNodes<<std::endl;
    }
    auto end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time;
    std::cout<<"Test Complete: "<<nodes<<" searched in "<<end_time<<"ms"<<std::endl;
    delete t_list;
}

/*----------------------------------------------Principal Variation Management--------------------------------------------------*/

void Search::storePvMove(const int move){
    int index = positionKey % pvTable->entries;
    pvTable->table[index].move = move;
    pvTable->table[index].positionKey = positionKey;
}

int Search::probePvTable(){
    int index = positionKey % pvTable->entries;
    if(pvTable->table[index].positionKey == positionKey){
        return pvTable->table[index].move;
    }
    return false;
}

bool Search::doesMoveExist(const int move){
    MoveList *list = new MoveList();
    GenerateMoves(list);
    for(int i = 0; i < list->count; i++){
        if(!makeMove(list->moves[i].move)){
            continue;
        }
        undoMove();
        if(list->moves[i].move == move){
            return true;
        }
    }
    return false;
}

int Search::getPvLine(const int depth){
    assert(depth < MaxDepth);
    int move = probePvTable();
    int count = 0;
    while(move != false && count < depth){
        if(doesMoveExist(move)){
            makeMove(move);
            pvArray[count] = move;
            count++;
        }else{
            break;
        }
        move = probePvTable();
    }
    while(ply > 0){
        undoMove();
    }
    return count;
}

/*----------------------------------------------Search Implementation--------------------------------------------------*/

//returns the score for the given position in one hundreds of a pawn(centipawns) from the SideToMove point of view
int Search::evaluate(){
    int piece{};
    int square{};
    //considering white's score as positive and Black's score as negative
    int score = materials[WHITE] - materials[BLACK];
    
//calculating pawns
    piece = WhitePawn;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score += PawnTable[orientation120to64[square]];
    }
    piece = BlackPawn;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score -= PawnTable[MirrorBlack[orientation120to64[square]]];
    }
//calculating knights
    piece = WhiteKnight;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score += KnightTable[orientation120to64[square]];
    }
    piece = BlackKnight;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score -= KnightTable[MirrorBlack[orientation120to64[square]]];
    }
//calculating Bishops
    piece = WhiteBishop;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score += BishopTable[orientation120to64[square]];
    }
    piece = BlackBishop;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score -= BishopTable[MirrorBlack[orientation120to64[square]]];
    }
//calculating Bishops
    piece = WhiteRook;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score += RookTable[orientation120to64[square]];
    }
    piece = BlackRook;
    for(int i = 0; i < pieceCount[piece]; i++){
        square = pieceList[piece][i];
        assert(isSquareOnBoard(square));
        score -= RookTable[MirrorBlack[orientation120to64[square]]];
    }
    
    if(sideToMove == WHITE){
        return score;
    }else{
        return  -(score);
    }
}

void Search::PickNextMove(int moveNumber, MoveList *list) {
    Move temp;
    int bestScore = 0;
    int bestNum = moveNumber;

    for (int index = moveNumber; index < list->count; ++index) {
        if (list->moves[index].score > bestScore) {
            bestScore = list->moves[index].score;
            bestNum = index;
        }
    }

    assert(moveNumber >= 0 && moveNumber < list->count);
    assert(bestNum >= 0 && bestNum < list->count);
    assert(bestNum >= moveNumber);

    temp = list->moves[moveNumber];
    list->moves[moveNumber] = list->moves[bestNum];
    list->moves[bestNum] = temp;
}

//checks if the time for searching is over or is there an interrupt from the GUI
void Search::checkStop(SearchInfo *info){
    long long now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if(info->timeset == true && (now > info->stopTime)){
        info->stopped = true;
    }
    ReadInput(info);
}

//clears the search and initiates for a new search
void Search::clearSearch(SearchInfo *info){
    for(int i = 0; i < array_size(searchHistory); i++){
        for(int j = 0; j < array_size(searchHistory[i]); j++){
            searchHistory[i][j] = 0;
        }
    }
    for(int i = 0; i < array_size(searchKillers); i++){
        for(int j = 0; j < array_size(searchKillers[i]); j++){
            searchKillers[i][j] = 0;
        }
    }
    clearPvTable(pvTable);
    ply = 0;
    info->stopped = 0;
    info->nodes = 0;
    info->failHigh = 0;
    info->failHighFirst = 0;
}

//handles the iterative deepening
void Search::searchPosition(SearchInfo *info){
    int bestMove = false;
    int bestScore = -Infinite;
    int currentDepth = 0;
    int pvMove = 0;
    clearSearch(info);
    //iterative deepening
    for(currentDepth = 1; currentDepth <= info->depth; ++currentDepth){
        //calling AlphaBeta
        bestScore = AlphaBeta(-Infinite, Infinite, currentDepth, info, true);
        if(info->stopped == true)
            break;
        pvMove = getPvLine(currentDepth);
        bestMove = pvArray[0];
        long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        if(info->gameMode == UCIMODE) {
            std::cout<<"info score cp "<<bestScore<<" depth "<<currentDepth<<" nodes "<<info->nodes<<" time "<<(endTime - info->startTime);
        } else if(info->gameMode == XBOARDMODE && info->postThinking == true) {
            std::cout<<currentDepth<<bestScore<<(endTime - info->startTime)/10<<info->nodes;
        } else if(info->postThinking == true) {
            std::cout<<"score: "<<bestScore<<"depth: "<<currentDepth<<"nodes: "<<info->nodes<<"time: "<<endTime - info->startTime<<"(ms)";
        }
        if(info->gameMode == UCIMODE || info->postThinking == true) {
            pvMove = getPvLine(currentDepth);
            std::cout<<" pv "<<pvMove<<" ";
            for(int pv = 0; pv < pvMove; pv++){
                std::cout<<PrintMove(pvArray[pv])<<"\t";
            }
            std::cout<<std::endl;
        }
        //std::cout<<"Ordering: "<<(info->failHighFirst/info->failHigh)<<std::endl;
    }
    if(info->gameMode == UCIMODE) {
        std::cout<<"bestmove: "<<PrintMove(bestMove)<<std::endl;
    }else if(info->gameMode == XBOARDMODE) {
        std::cout<<"move "<<PrintMove(bestMove)<<std::endl;
        makeMove(bestMove);
    }else{
        std::cout<<"\n\n***!! Senso makes move "<<PrintMove(bestMove)<<" !!***\n\n";
        makeMove(bestMove);
        PrintBoard();
    }
}

//doesn't handle Horizon Effect, need to implement Quiescence Search
int Search::AlphaBeta(int alpha, int beta, int depth, SearchInfo *info, bool nullMove){
    assert(CheckBoard());
    if(depth == 0){
        return quiescenceSearch(alpha, beta, info);
    }
    if((info->nodes & 2047) == 0){
        checkStop(info);
    }
    info->nodes++;
    
    if((isRepeated() || fiftyMoveCounter >= 100) && ply){
        return 0;
    }
    if(ply > MaxDepth-1){
        return evaluate();
    }
    int inCheck = attackingSquare(kings[sideToMove], sideToMove^1);
    if(inCheck){
        depth++;
    }
    MoveList *list = new MoveList();
    GenerateMoves(list);
    
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = false;
    int score = -Infinite;
    int pvMove = probePvTable();
    
    if(pvMove != false){
        for(int i = 0; i < list->count; ++i){
            if(list->moves[i].move == pvMove){
                list->moves[i].score = 2000000;
                break;
            }
        }
    }
    
    for(int i = 0; i < list->count; i++){
        PickNextMove(i, list);
        if(!makeMove(list->moves[i].move)){
            continue;
        }
        legal++;
        score = -(AlphaBeta(-beta, -alpha, (depth - 1), info, true));
        undoMove();
        
        if(info->stopped == true)
            return 0;
        
        if(score > alpha){
            if(score >= beta){
                //we have reached a beta cutoff, so the rest needn't be searched
                if(legal == 1){
                    info->failHighFirst++;
                }
                info->failHigh++;
                if(!(list->moves[i].move & flagCapture)){
                    searchKillers[1][ply] = searchKillers[0][ply];
                    searchKillers[0][ply] = list->moves[i].move;
                }
                return beta;
            }
            alpha = score;
            bestMove = list->moves[i].move;
            if(!(list->moves[i].move & flagCapture)){
                searchHistory[pieces[getFromSquare(bestMove)]][getToSquare(bestMove)] += depth;
            }
        }
    }
    
    if(legal == 0){
        if(inCheck){
            return -MATE + ply;
        }else
            return 0;
    }
    
    if(alpha >= oldAlpha){
        storePvMove(bestMove);
    }
    return alpha;
}

int Search::quiescenceSearch(int alpha, int beta, SearchInfo *info){
    assert(CheckBoard());
    if((info->nodes & 2047) == 0){
        checkStop(info);
    }
    info->nodes++;
    
    if(isRepeated() || fiftyMoveCounter >= 100)
        return 0;
    if(ply > (MaxDepth - 1))
        return evaluate();
    
    int score = evaluate();
    if(score >= beta){
        return beta;
    }
    if(score > alpha){
        alpha = score;
    }
    
    MoveList *list = new MoveList();
    GenerateCaptureMoves(list);
    
    int legal = 0;
    int oldAlpha = alpha;
    int bestMove = false;
    score = -Infinite;
    
    for(int i = 0; i < list->count; i++){
        PickNextMove(i, list);
        if(!makeMove(list->moves[i].move)){
            continue;
        }
        legal++;
        score = -(quiescenceSearch(-beta, -alpha, info));
        undoMove();
        if(score > alpha){
            if(score >= beta){
                //we have reached a beta cutoff, so the rest needn't be searched
                if(legal == 1){
                    info->failHighFirst++;
                }
                info->failHigh++;
                return beta;
            }
            alpha = score;
            bestMove = list->moves[i].move;
        }
    }
    
    if(alpha >= oldAlpha){
        storePvMove(bestMove);
    }
    return alpha;
}

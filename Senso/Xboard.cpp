//
//  Xboard.cpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/12/21.
//

#include "Xboard.hpp"

int threeFoldRep(const std::shared_ptr<Search> Test){
    int repetition{};
    for(int i = 0; i < Test->totalPly; i++){
        if(Test->history[i].positionKey == Test->positionKey)
            repetition++;
    }
    return repetition;
}

int drawMaterials(const std::shared_ptr<Search> Test){
    if(Test->pieceCount[WhitePawn] || Test->pieceCount[BlackPawn]) return false;
    if(Test->pieceCount[WhiteQueen] || Test->pieceCount[BlackQueen] || Test->pieceCount[WhiteRook] || Test->pieceCount[BlackRook]) return false;
    if((Test->pieceCount[WhiteBishop] > 1) || (Test->pieceCount[BlackBishop] > 1)) return false;
    if((Test->pieceCount[WhiteKnight] > 1) || (Test->pieceCount[BlackKnight] > 1)) return false;
    if(Test->pieceCount[WhiteBishop] && Test->pieceCount[WhiteKnight]) return false;
    if(Test->pieceCount[BlackKnight] && Test->pieceCount[BlackBishop]) return false;
    return true;
}

int checkForDraw(const std::shared_ptr<Search> Test){
    if(Test->fiftyMoveCounter > 100){
        std::cout<<"1/2-1/2 {Fifty Move rule (claimed by Senso)}"<<std::endl;
        return true;
    }
    if(threeFoldRep(Test) > 2){
        std::cout<<"1/2-1/2 {Three fold repetition (claimed by Senso)}"<<std::endl;
        return true;
    }
    if(drawMaterials(Test)){
        std::cout<<"1/2-1/2 {Three fold repetition (claimed by Senso)}"<<std::endl;
        return true;
    }
    
    //check for CheckMate and StaleMate
    bool mate = false;
    MoveList *list = new MoveList();
    Test->GenerateMoves(list);
    for(int i = 0; i < list->count; ++i){
        if(!Test->makeMove(list->moves[i].move))
            continue;
        mate = true;
        Test->undoMove();
        break;
    }
    if(mate)
        return false;
    //look if sideToPlay is in Check
    int check = Test->attackingSquare(Test->kings[Test->sideToMove], (Test->sideToMove ^  1));
    if(check){
        if(Test->sideToMove == WHITE){
            std::cout<<"0-1 {black mates (claimed by Senso)}"<<std::endl;
            return true;
        }else{
            std::cout<<"0-1 {white mates (claimed by Senso)}"<<std::endl;
            return true;
        }
    }else{
        std::cout<<"1/2-1/2 {stalemate (claimed by Senso)}"<<std::endl;
        return true;
    }
    return false;
}

void PrintOptions() {
    std::cout<<"feature ping=1 setboard=1 colors=0 usermove=1 memory=1\n";
    std::cout<<"feature done=1\n";
}

/*----------------------------------------------Xboard Implementation--------------------------------------------------*/


void xboardRun(const std::shared_ptr<Search>& Test){
    Test->info->gameMode = XBOARDMODE;
    Test->info->postThinking = true;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    PrintOptions();
    
    int depth = -1;
    int movestogo[2] = {30, 30};
    int movetime = -1;
    int time = -1;
    int increment = 0;
    int engineSide = BOTH;
    int timeLeft{};
    int movesPerSecond{};
    int move = false;
    //int score{};
    std::string inputBuffer{};
    std::string command{};
    int seconds{};
    
    engineSide = BLACK;
    Test->parseFEN(startingFEN);
    depth = -1;
    time = -1;
    
    while(true){
        fflush(stdout);
        if(Test->sideToMove == engineSide && (checkForDraw(Test) == false)){
            //make the move
            Test->info->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            Test->info->depth = depth;

            if(time != -1){
                Test->info->timeset = true;
                time /= movestogo[Test->sideToMove];
                time -= 50;
                Test->info->stopTime = Test->info->startTime + time + increment;
            }

            if(depth == -1 || depth > MaxDepth){
                Test->info->depth = MaxDepth;
            }

            std::cout<<"time:"<<time<<"start:"<<Test->info->startTime<<"stop:"<<Test->info->stopTime<<"depth:"<<Test->info->depth<<"timeset:"<<Test->info->timeset<<"movestogo:"<<movestogo[Test->sideToMove]<<"mps:"<<movesPerSecond<<"\n";
                Test->searchPosition(Test->info);

            if(movesPerSecond != 0) {
                movestogo[Test->sideToMove^1]--;
                if(movestogo[Test->sideToMove^1] < 1) {
                    movestogo[Test->sideToMove^1] = movesPerSecond;
                }
            }
        }
        
        fflush(stdout);
        inputBuffer.clear();
        fflush(stdout);
        getline(std::cin, inputBuffer);
        if(!inputBuffer[0]){
            continue;
        }
        command = inputBuffer.substr(0, inputBuffer.find(" "));
        //processing the command
        if(command == "quit") {
            Test->info->quit = true;
            break;
        }

        if(command == "force") {
            engineSide = BOTH;
            continue;
        }

        if(command == "protover"){
            PrintOptions();
            continue;
        }

        if(command == "sd") {
            depth = stoi(inputBuffer.substr(inputBuffer.find("sd ")+3));
            std::cout<<"DEBUG depth: "<<depth<<std::endl;
            continue;
        }

        if(command == "st") {
            movetime = stoi(inputBuffer.substr(inputBuffer.find("st ")+3));
            std::cout<<"DEBUG movetime: "<<movetime<<std::endl;
            continue;
        }

        if(command == "time") {
            time = stoi(inputBuffer.substr(inputBuffer.find("time ")+5));
            time *= 10;
            std::cout<<"DEBUG time:"<<time<<"\n";
            continue;
        }

        if(command == "level") {
            seconds = 0;
            movetime = -1;
            if( sscanf(inputBuffer.c_str(), "level %d %d %d", &movesPerSecond, &timeLeft, &increment) != 3) {
              sscanf(inputBuffer.c_str(), "level %d %d:%d %d", &movesPerSecond, &timeLeft, &seconds, &increment);
              std::cout<<"DEBUG level with :\n";
            }else{
              std::cout<<"DEBUG level without :\n";
            }
            timeLeft *= 60000;
            timeLeft += seconds * 1000;
            movestogo[0] = movestogo[1] = 30;
            if(movesPerSecond != 0) {
                movestogo[0] = movestogo[1] = movesPerSecond;
            }
            time = -1;
            printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],increment,movesPerSecond);
            continue;
        }

        if(command == "ping") {
            std::cout<<"pong"<<inputBuffer.substr(4)<<std::endl;
            continue;
        }

        if(command == "new") {
            //ClearHashTable(pos->HashTable);
            engineSide = BLACK;
            Test->parseFEN(startingFEN);
            depth = -1;
            time = -1;
            continue;
        }

        if(command == "setboard"){
            engineSide = BOTH;
            Test->parseFEN(inputBuffer.substr(9));
            continue;
        }

        if(command == "go") {
            engineSide = Test->sideToMove;
            continue;
        }

        if(command == "usermove"){
            movestogo[Test->sideToMove]--;
            move = Test->parseMove(inputBuffer.substr(9));
            if(move == false) continue;
            Test->makeMove(move);
            Test->ply = 0;
        }
    }
    
}

/*----------------------------------------------Console Mode Implementation--------------------------------------------------*/


void consoleRun(const std::shared_ptr<Search> Test){

    std::cout<<"Welcome to Senso In Console Mode!"<<std::endl;
    std::cout<<"Type help for commands\n\n";

    Test->info->gameMode = CONSOLEMODE;
    Test->info->postThinking = true;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int depth = MaxDepth;
    int movetime = 3000;
    int engineSide = BOTH;
    int move = false;
    std::string inputBuffer{};
    std::string command{};

    engineSide = BLACK;
    Test->parseFEN(startingFEN);

    while(true) {

        fflush(stdout);

        if(Test->sideToMove == engineSide && (checkForDraw(Test) == false)) {
            Test->info->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            Test->info->depth = depth;

            if(movetime != 0) {
                Test->info->timeset = true;
                Test->info->stopTime = Test->info->startTime + movetime;
            }

            Test->searchPosition(Test->info);
        }

        std::cout<<"\nSenso > ";

        fflush(stdout);

        inputBuffer.clear();
        fflush(stdout);
        getline(std::cin, inputBuffer);
        if(!inputBuffer[0]){
            continue;
        }
        command = inputBuffer.substr(0, inputBuffer.find(" "));

        if(command == "help"){
            std::cout<<"Commands:\n";
            std::cout<<"quit - quit game\n";
            std::cout<<"force - computer will not think\n";
            std::cout<<"print - show board\n";
            std::cout<<"post - show thinking\n";
            std::cout<<"nopost - do not show thinking\n";
            std::cout<<"new - start new game\n";
            std::cout<<"go - set computer thinking\n";
            std::cout<<"depth x - set depth to x\n";
            std::cout<<"time x - set thinking time to x seconds (depth still applies if set)\n";
            std::cout<<"view - show current depth and movetime settings\n";
            std::cout<<"setboard x - set position to fen x\n";
            std::cout<<"** note ** - to reset time and depth, set to 0\n";
            std::cout<<"enter moves using b7b8q notation\n\n\n";
            continue;
        }

        if(command == "eval"){
            Test->PrintBoard();
            std::cout<<"Eval:"<<Test->evaluate();
            //MirrorBoard(pos);
            Test->PrintBoard();
            std::cout<<"Eval:"<<Test->evaluate();
            continue;
        }

        if(command == "setboard"){
            engineSide = BOTH;
            Test->parseFEN(inputBuffer.substr(9));
            continue;
        }

        if(command == "quit"){
            Test->info->quit = true;
            break;
        }

        if(command == "post"){
            Test->info->postThinking = true;
            continue;
        }

        if(command == "print") {
            Test->PrintBoard();
            continue;
        }

        if(command == "nopost") {
            Test->info->postThinking = false;
            continue;
        }

        if(command == "force") {
            engineSide = BOTH;
            continue;
        }

        if(command == "view") {
            if(depth == MaxDepth)
                std::cout<<"depth not set ";
            else
                std::cout<<"depth "<<depth;

            if(movetime != 0)
                std::cout<<"movetime "<<movetime/1000<<std::endl;
            else
                std::cout<<" movetime not set\n";
            continue;
        }

        if(command == "depth"){
            sscanf(inputBuffer.c_str(), "depth %d", &depth);
            if(depth == 0)
                depth = MaxDepth;
            continue;
        }

        if(command == "time") {
            sscanf(inputBuffer.c_str(), "time %d", &movetime);
            movetime *= 1000;
            continue;
        }

        if(command == "new") {
            //ClearHashTable(pos->HashTable);
            engineSide = BLACK;
            Test->parseFEN(startingFEN);
            continue;
        }

        if(command == "go") {
            engineSide = Test->sideToMove;
            continue;
        }

        move = Test->parseMove(inputBuffer);
        if(move == false){
            std::cout<<"Command unknown: "<<inputBuffer<<std::endl;
            continue;
        }
        Test->makeMove(move);
        Test->ply = 0;
    }
}

//
//  UCI.cpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/11/21.
//

#include "UCI.hpp"

void parseGo(std::string line, SearchInfo *info, const std::shared_ptr<Search> Test){
    int depth = -1;
    int movestogo = 30;
    int movetime = -1;
    int time = -1;
    int increment = 0;
    std::size_t size{};
    std::size_t position{};
    info->timeset = false;
    
    if((position = line.find("infinite")) != std::string::npos){
        ;
    }
    if((position = line.find("binc")) != std::string::npos && Test->sideToMove == BLACK){
        increment = std::stoi(line.substr(position + 5), &size);
    }
    if((position = line.find("winc")) != std::string::npos && Test->sideToMove == WHITE){
        increment = std::stoi(line.substr(position + 5), &size);
    }
    if((position = line.find("btime")) != std::string::npos && Test->sideToMove == BLACK){
        time = std::stoi(line.substr(position + 6), &size);
    }
    if((position = line.find("wtime")) != std::string::npos && Test->sideToMove == WHITE){
        time = std::stoi(line.substr(position + 6), &size);
    }
    if((position = line.find("movestogo")) != std::string::npos){
        movestogo = std::stoi(line.substr(position + 10), &size);
    }
    if((position = line.find("movetime")) != std::string::npos){
        movetime = std::stoi(line.substr(position + 9), &size);
    }
    if((position = line.find("depth")) != std::string::npos){
        depth = std::stoi(line.substr(position + 6), &size);
    }
    //changing the values based on the above string parsed
    if(movetime != -1){
        time = movetime;
        movestogo = 1;
    }
    info->startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    info->depth = depth;
    
    if(time != -1){
        info->timeset = true;
        time /= movestogo;
        time -= 50; //subtracting 50ms just to make sure we don't run over the alloted time
        info->stopTime = info->startTime + time + increment;
    }
    //if depth wasn't set, reset it to maxdepth
    if(depth == -1){
        info->depth = MaxDepth;
    }
    std::cout<<"time: "<<time<<" start: "<<info->startTime<<" stop: "<<info->stopTime<<" depth: "<<info->depth<<" timeset: "<<info->timeset<<std::endl;
    Test->searchPosition(info);

}

void parsePosition(std::string line, const std::shared_ptr<Search> Test){
    int index = 9; //as every line start with a "position " followed by the command that has to be parsed
    if(line.compare(9, 8, "startpos") == 0){
        Test->parseFEN(startingFEN);
    }else{
        if(line.compare(9, 3, "fen") == 0){
            index += 4;
            std::string tempLine = line.substr(index);
            Test->parseFEN(tempLine);
        }else{
            Test->parseFEN(startingFEN);
        }
    }
    //parsing the moves
    std::size_t position = line.find("moves");
    if(position != std::string::npos){
        std::string tempLine = line.substr(position + 6);
        index = 0;
        int move{};
        if(tempLine.size()){
            while(index < tempLine.size()){
                move = Test->parseMove(tempLine);
                if(move == false)
                    break;
                Test->makeMove(move);
                Test->ply = 0;
                while(tempLine[index] != ' ' && index < tempLine.size())
                    index++;
                if(tempLine[index] == ' '){
                    tempLine = tempLine.substr(++index);
                    index = 0;
                }
            }
        }
    }
    Test->PrintBoard();
}

void uciRun(const std::shared_ptr<Search> Test){
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    
    std::string line{};
    std::cout<<"id name "<<Name<<std::endl;
    std::cout<<"id author "<<Author<<std::endl;

    while(true){
        line.clear();
        fflush(stdout);
        getline(std::cin, line);
        if (!(line[0])){
            continue;
        }
        if (line[0] == '\n'){
            continue;
        }

        if (!(line.compare("isready"))){
            std::cout<<"readyok\n";
            continue;
        } else if (line.find("position") != std::string::npos){
            parsePosition(line, Test);
        } else if (line.find("ucinewgame") != std::string::npos){
            parsePosition("position startpos\n", Test);
        } else if (line.find("go") != std::string::npos){
            std::cout<<"Thinking..."<<std::endl;
            parseGo(line, Test->info, Test);
        } else if (!(line.compare("quit"))){
            Test->info->quit = true;
            break;
        } else if (!(line.compare("uci"))){
            std::cout<<"id name "<<Name<<std::endl;
            std::cout<<"id author "<<Author<<std::endl;
            std::cout<<"uciok\n";
        }

        if(Test->info->quit){
            break;
        }
    }
}

int InputWaiting(){
    #ifndef WIN32
      fd_set readfds;
      struct timeval tv;
      FD_ZERO (&readfds);
      FD_SET (fileno(stdin), &readfds);
      tv.tv_sec=0; tv.tv_usec=0;
      select(16, &readfds, 0, 0, &tv);

      return (FD_ISSET(fileno(stdin), &readfds));
    #else
       static int init = 0, pipe;
       static HANDLE inh;
       DWORD dw;

       if (!init) {
         init = 1;
         inh = GetStdHandle(STD_INPUT_HANDLE);
         pipe = !GetConsoleMode(inh, &dw);
         if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
          }
        }
        if (pipe) {
          if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
          return dw;
        } else {
          GetNumberOfConsoleInputEvents(inh, &dw);
          return dw <= 1 ? 0 : dw;
        }
    #endif
}

void ReadInput(SearchInfo *info){
    int bytes;
    char input[256] = "", *endc;

    if (InputWaiting()) {
        info->stopped = true;
        do{
          bytes = static_cast<int>(read(fileno(stdin),input,256));
        }while (bytes<0);
        endc = strchr(input,'\n');
        if (endc) *endc=0;

        if (strlen(input) > 0) {
            if (!strncmp(input, "quit", 4))    {
              info->quit = true;
            }
        }
        return;
    }
}

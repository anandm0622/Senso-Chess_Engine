//
//  main.cpp
//  Senso
//
//  Created by Anand Manikandan on 2/27/21.
//

#include "Search.hpp"
#include "UCI.hpp"
#include "Xboard.hpp"

//"r5k1/1bpnqrpp/pp2p3/3p4/N1PPnb2/1P1B1N2/PBR1QPPP/3R2K1/ b - 0 1"

#define FEN1 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define FEN2 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"
#define FEN3 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

void uciRun(const std::shared_ptr<Search> Test);
void xboardRun(const std::shared_ptr<Search>& Test);
void consoleRun(const std::shared_ptr<Search> Test);

int main(int argc, const char * argv[]) {
    
    auto Test = std::make_shared<Search>();
    Test->initBoard();
    
    std::cout<<"Type 'console' for console mode"<<std::endl;
    std::string line{};
    while(true){
        line.clear();
        fflush(stdout);
        getline(std::cin, line);
        if(!line[0])
            continue;;
        if(line[0] == '\n')
            continue;
        if(line.find("uci") != std::string::npos){
            uciRun(Test);
            if(Test->info->quit == true)
                break;
            continue;
        }else if(line.find("xboard") != std::string::npos){
            xboardRun(Test);
            if(Test->info->quit == true)
                break;
            continue;
        }else if(line.find("console") != std::string::npos){
            consoleRun(Test);
            if(Test->info->quit == true)
                break;
            continue;
        }else if (line.find("quit") != std::string::npos){
            break;
        }
    }
    
    
    
    return 0;
}



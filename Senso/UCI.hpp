//
//  UCI.hpp
//  Senso_temp
//
//  Created by Anand Manikandan on 3/11/21.
//

#pragma once

#include "Search.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

//void uciRun(const std::shared_ptr<Search> Test);
//void parsePosition(std::string line, const std::shared_ptr<Search> Test);
//void parseGo(std::string line, SearchInfo *info, const std::shared_ptr<Search> Test);
void ReadInput(SearchInfo *info);
int InputWaiting();

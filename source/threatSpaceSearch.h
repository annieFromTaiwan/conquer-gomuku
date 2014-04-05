#ifndef THREATSPACESEARCH_H
#define THREATSPACESEARCH_H

#include <algorithm>
#include <vector>
#include "err.h"
#include "boardData.h"
#include "threat.h"
#include "findThreat.h"
#include "paper.h"

#define STOP_SEARCH_IF_WIN      // in threatSpaceSearch() & threatSpaceSearchMain()
#define PRINT_TEST           // in threatSpaceSearch()
#define PRINT_NORMAL_THREAT  // in threatSpaceSearchMain()

#define FLAG_TSS_true 1

#define THREAT_SEQ_MAX 2

int threatSpaceSearch(BoardData *brd, int turn, vector< pair<int,int> > &ans, Paper *paper=NULL);


#endif // THREATSPACESEARCH_H

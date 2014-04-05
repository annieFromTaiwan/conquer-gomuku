#ifndef PROOFNUMBERSEARCH_H
#define PROOFNUMBERSEARCH_H

#include "err.h"
#include <vector>
#include "BoardData.h"
#include "threatSpaceSearch.h"

#include <iostream>
#include <algorithm>
#include <stdlib.h> //genChildrenList(), genChildrenListCmp()

// the infinity value of pn-search
#define PNS_INFINITY 1000000


struct pnsNode {
    BoardData *brd;
    int turn;
    int pn;
    int dn;
    int x; // useful only when testing
    int y; // useful only when testing
    pnsNode *parent;
    vector< pair<int,int> > virtualChildren; // not yet malloc, only coord
    vector<pnsNode*> actualChildren;

    vector< pair<int,int> > winningSeq;

    pnsNode(BoardData *b, int t){
        brd = new BoardData(b);
        turn = t;
        pn = 1;
        dn = 1;
        x = -1;
        y = -1;
        parent = NULL;
    }

    pnsNode(pnsNode *_parent, const pair<int,int> coord){
        parent = _parent;
        brd = new BoardData(parent->brd);
        turn = opposite(parent->turn);
        x = coord.first;
        y = coord.second;
        brd->set(x, y, parent->turn);
        pn = 1;
        dn = 1;
    }
} ;

void genChildrenList(pnsNode *n);

pnsNode* updatePnDn(pnsNode *node, int pnBefore, int dnBefore, int rootTurn);

// return value is the most-proving-node
// ( return NULL if not in current tree )
pnsNode* proofNumberSearch(pnsNode *node, int rootTurn, bool runTSS, Paper *paper=NULL);

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
void buildTreeWidgetItem(pnsNode *node, QTreeWidgetItem *rootItem);

void printTreePnDn(pnsNode *node, Paper *paper);

void printWinningTree(pnsNode *node, Paper *paper);

int getTruthByPNS(BoardData *brd, int turn, int turnToBeProved, bool runTSS, pair<int,int> *ans, Paper *paper=NULL, pnsNode** rootNode=NULL);

#endif // PROOFNUMBERSEARCH_H

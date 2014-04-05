#include "threatSpaceSearch.h"
//#include "proofNumberSearch.h"

struct pnsNode;
extern int getTruthByPNS(BoardData *brd, int turn, int turnToBeProved, bool runTSS, pair<int,int> *ans, Paper *paper=NULL, pnsNode** rootNode=NULL);
BoardData *g_origBrd;
pair<int, int> g_threatSeq[THREAT_SEQ_MAX+1];
int g_threatSeqLen = 0;

// make sure (turn) can attack and win as scheduled
int verify(BoardData *brd, int turn, int now, Paper *paper=NULL)
{
    /******** play scheduled (turn)-square OR defend (!turn) *********/
    vector< pair<int, int> > my = availableMove(new BoardData(brd), turn);

    if(my.size()==0){
        safePrint(paper, "(m%s,noAvailableMove) ",(turn==BLACK)?"B":"W");
        return NOWIN;
    // scheduled (turn)-attack square is available
    }else if( std::find(my.begin(), my.end(), g_threatSeq[now] ) !=my.end() ){
        brd->set(g_threatSeq[now].first, g_threatSeq[now].second, turn);
        safePrint(paper, "(m%s,%d,%d) ",(turn==BLACK)?"B":"W", g_threatSeq[now].first, g_threatSeq[now].second);
        now++;
        if(now==g_threatSeqLen){
            safePrint(paper, " gtbPNS() ");
            return getTruthByPNS(new BoardData(brd), opposite(turn), turn, false, NULL);
        }

        // play all (!turn)-moves
        vector< pair<int, int> > opn = availableMove(new BoardData(brd), opposite(turn));
        if(opn.size()==0)
            return WIN;
        // (!turn) can play scheduled (turn)-attack-square
        else if( std::find(opn.begin(), opn.end(), g_threatSeq[now])!=opn.end() )
            return NOWIN;
        else for(uint o=0;o<opn.size();o++){
            brd->set(opn[o].first, opn[o].second, opposite(turn));
            safePrint(paper, "%d(o%s,%d,%d) ",o,(opposite(turn)==BLACK)?"B":"W",opn[o].first, opn[o].second);
            safePrint(paper, "verify(): [ ");
            if( verify(new BoardData(brd), turn, now, paper) ==NOWIN){
                safePrint(paper, "NOWIN ]");
                return NOWIN;
            }
            safePrint(paper, " ]");
            brd->set(opn[o].first, opn[o].second, EMPTY);
        }
    }
    // being forced to defend (!turn)
    else {
        //there probably has no (!turn) threats but still can't play scheduled attack move
        if(my.size()>30){
            safePrint(paper, "(m%s,%d,%d,isNotAvailable) ",(turn==BLACK)?"B":"W", g_threatSeq[now].first, g_threatSeq[now].second);
            return NOWIN;
        }

        //play all possible (turn)-defensive moves
        for(uint m=0; m<my.size(); m++){
            brd->set(my[m].first, my[m].second, turn);
            safePrint(paper, "%d(m%s,%d,%d) ",m,(turn==BLACK)?"B":"W",my[m].first, my[m].second);

            // play all (!turn)-moves
            vector< pair<int, int> > opn = availableMove(new BoardData(brd), opposite(turn));
            // (!turn) can play scheduled (turn)-attack-square
            if( std::find(opn.begin(), opn.end(), g_threatSeq[now])!=opn.end() )
                return NOWIN;
            else for(uint o=0;o<opn.size();o++){
                brd->set(opn[o].first, opn[o].second, opposite(turn));
                safePrint(paper, "%d(o%s,%d,%d) ",o,(opposite(turn)==BLACK)?"B":"W",opn[o].first, opn[o].second);
                safePrint(paper, "[ ");
                if( verify(new BoardData(brd), turn, now, paper) ==NOWIN){
                    safePrint(paper, "NOWIN ]");
                    return NOWIN;
                }
                safePrint(paper, " ]");
                brd->set(opn[o].first, opn[o].second, EMPTY);
            }

            brd->set(my[m].first, my[m].second, EMPTY);
        }
    }

    return WIN;
}
// int x, y : last threat's gain square -> need to be one of the current threat's rest squares
int threatSpaceSearchMain(BoardData *brd, int turn, int x, int y, int depth, Paper *paper=NULL)
{
    int winValue = NOWIN;

    /*
    Threat *opn = findExistingMaxThreat(new BoardData(brd), opposite(turn));
#ifdef PRINT_TEST2
    if(opn!=NULL){
        for(int i=0;i<depth;i++)safePrint(paper, " ");
        safePrint(paper, "X");
        opn->printThreat();
        if(opn->type==StrT) safePrint(paper, "========%d %d %d\n",turn,x,y);
    }
#endif*/

    vector< Threat* > my = findAllThreatWithRest(new BoardData(brd), turn, x, y);

    /*************************************************************************
     * search all the threats down
     *************************************************************************/
    for(uint no=0; no<my.size(); no++){
        Threat *t = my[no];

        for(int i=0;i<depth;i++)safePrint(paper, " ");
        t->printThreat(paper);

        /********   start to search down all possible threat sequence   ********/
        if(t->type==Five || t->type==StrF){
            // if the threat is already a winning threat, then don't search down
            g_threatSeqLen = depth;
            safePrint(paper, "===verify():");
            // because that verify() will change [tmp], so we copy [g_origBrd] everytime
            /*BoardData *tmp = new BoardData(g_origBrd);
            winValue = verify(tmp,turn,0,paper); // start search from depth = 0
            delete tmp;*/
            winValue = verify(new BoardData(g_origBrd),turn,0,paper); // start search from depth = 0
            safePrint(paper, "===verify(): %s ===\n", (winValue==WIN)? "WIN" : "NOWIN");
        }else if( depth < THREAT_SEQ_MAX ){
            // if depth too much, then stop tsSearch
            // put 'stop' here, to accelerate a little more

            // start to search down all possible threat sequence
            // preset
            g_threatSeq[depth] = make_pair( t->getGainX(), t->getGainY() );
            brd->set(t->getGainX(), t->getGainY(), turn);
            for(int i=0; i<t->costNum; i++) brd->set(t->getCostX(i), t->getCostY(i), opposite(turn));
            // search
            winValue = threatSpaceSearchMain(new BoardData(brd), turn, t->getGainX(), t->getGainY(), depth+1, paper);
            // reset
            brd->set(t->getGainX(), t->getGainY(), EMPTY);
            for(int i=0; i<t->costNum; i++) brd->set(t->getCostX(i), t->getCostY(i), EMPTY);
        }
        delete t;
#ifdef STOP_SEARCH_IF_WIN
        if( winValue == WIN ) break;
#endif
    }

    return winValue;
}

// the entry of threat-space search
//pair<int,int> threatSpaceSearch(BoardData *brd, int turn)
int threatSpaceSearch(BoardData *brd, int turn, vector<pair<int, int> > &ans, Paper *paper)
{
    //brd->printBoard(paper);
    safePrint(paper, "vvvvv tsSearch, %s vvvvv\n", (turn==BLACK)?"BLACK":"WHITE" );

    int winValue = NOWIN;
    g_origBrd = new BoardData(brd);
    g_threatSeqLen = 0;

    // find all [turn]'s future threats which are bigger than opn
    Threat *opn = findMaxThreat( brd, opposite(turn), false ); //gainSquareIsEmpty=false
    vector< Threat* > my = findAllThreat(new BoardData(brd), turn, true, opn); //gainSquareIsEmpty=true

    for(uint no=0; no<my.size(); no++){
        Threat *t = my[no];
        t->printThreat(paper);

        if(t->type==Five || t->type==StrF){
            // if the threat is already a winning threat, then don't search down
            winValue = WIN;
            g_threatSeq[0] = make_pair( t->getGainX(), t->getGainY() );
            g_threatSeqLen = 1;
        }else{
            // start to search down all possible threat sequence
            // preset
            g_threatSeq[0] = make_pair( t->getGainX(), t->getGainY() );
            brd->set(t->getGainX(), t->getGainY(), turn);
            for(int i=0; i<t->costNum; i++) brd->set(t->getCostX(i), t->getCostY(i), opposite(turn));
            // search
            winValue = threatSpaceSearchMain(new BoardData(brd), turn, t->getGainX(), t->getGainY(), 1, paper); //depth=1
            // reset
            brd->set(t->getGainX(), t->getGainY(), EMPTY);
            for(int i=0; i<t->costNum; i++) brd->set(t->getCostX(i), t->getCostY(i), EMPTY);
        }
        delete t;
#ifdef STOP_SEARCH_IF_WIN
        if( winValue == WIN ) break;
        //if( winValue == WIN ) return g_threatSeq;
#endif
    }

    ans.clear();
    for(int i=0; i<g_threatSeqLen; i++)
        ans.push_back(g_threatSeq[i]);
    safePrint(paper, "^^^^^ tsSearch, %s ^^^^^\n", (winValue==WIN)?"WIN":"NOWIN" );
    return winValue;
    //if( winValue == WIN ) return g_threatSeq;
    //else return NULL;
}


#include "findThreat.h"
#include "stdio.h"

void printLine(vector <unsigned char> queue)
{
    printf("[[");

    for(vector<unsigned char>::iterator it = queue.begin();
            it != queue.end();
            it++)
        switch(*it){
            case EMPTY: printf("_"); break;
            case WHITE: printf("O"); break;
            case BLACK: printf("X"); break;
            case BOUND: printf("#"); break;
        }

    printf("]]\n");
}


// Finds a threat who
//   has (x,y), which is an empty/turn square, to be a gain square, and
//   has some point (restX,restY) to be its rest square (done by using [restMask]).
// And to save time, it should only search for the specified direction
//   (the direction - (x,y) to (restX,restY)) (done by using [dx],[dy])
// 尋找在(dx,dy)方向，若下了(x,y)以後/若以(x,y)為gain square，會不會形成threat
// 不會的話return Null，會的話return那個threat
// 可以指定須包含哪個rest square
//  (x,y)必須為EMPTY或turn
Threat* checkThreat(BoardData *brd,int turn,int x,int y,int dx,int dy,unsigned short int restMask)
{
    if( brd->get(x,y) != EMPTY && brd->get(x,y) != turn ) return NULL;

#ifdef RECORD_THREAT
    // to record the stones in the whole straight line
    vector <unsigned char> line;
#endif

    // gets the value of the line, and store them into 16 bits
    // transform BoardData value into a 2-byte short int
    unsigned short int decodeBytes = 0b0000000000000000;
    int cnt, tmpx, tmpy;
    for(cnt=9, tmpx=x-4*dx, tmpy=y-4*dy;   cnt>0;   cnt--, tmpx+=dx, tmpy+=dy){
        if(tmpx == x && tmpy == y){
#ifdef RECORD_THREAT
            // imagine brd[x][y] (original empty) is being put on a 'turn' stone
            line.push_back(turn);
#endif
            continue;
        }
        unsigned char square = brd->get(tmpx,tmpy);
        unsigned char decodeBits;
#ifdef RECORD_THREAT
        line.push_back(square);
#endif
        if( square==BOUND || square==opposite(turn) )
            decodeBits = 0b01;
        else if ( square==EMPTY )
            decodeBits = 0b10;
        else if ( square==turn )
            decodeBits = 0b11;
        else err (true, "checkThreat() BoardData value error: at (%d,%d)", tmpx, tmpy);
        // add decodeBits(2-bit) into the tail of decodeBytes(total 2-byte)
        decodeBytes <<= 2;
        decodeBytes += decodeBits ;
    }
#ifdef RECORD_THREAT
    printLine(line);
    printf("%x\n", decodeBytes);
#endif

    Threat *ans = NULL;
    for(int i=0; i<BasicThreatListLen; i++){
        if( ( BasicThreatList(i)->mask & restMask ) == 0 )
            continue;
        if( ( decodeBytes & BasicThreatList(i)->mask) == BasicThreatList(i)->seqb ){
            ans = new Threat(x,y,dx,dy,BasicThreatList(i));
#ifdef RECORD_THREAT
            ans->printThreat();
#endif
            break;
        }
    }
#ifdef RECORD_THREAT
    line.clear();
#endif
    return ans;
}



int directions[4][2] = {{1,1},{1,0},{1,-1},{0,1}} ;
// 找到所有以(restX,restY)為rest square的threats
// 這些threats的gain square必須為EMPTY
vector< Threat* > findAllThreatWithRest(BoardData *brd,int turn,int restX,int restY)
{
    vector< Threat* > threatList = vector< Threat* >();

    /*************************************************************************
     * find all the possible threats that contains (x,y) as its rest square
     *************************************************************************/
    // four directions: 0(horizontal),45,90(vertical),135 degrees
    for(int dir=0; dir<4; dir++){
        int dx=directions[dir][0], dy=directions[dir][1];
        // the mask comes from
        //   the relative place of
        //     the specified rest square and the candidate gain square
        // which is used to examine
        //   whether the candidate threat type contains the specified rest square
        unsigned short int restSquareMask = 0b0000000000000011;

        // if (tmpx,tmpy) is EMPTY then it is the candidate gain square of the current threat
        // ( EMPTY is examined in findThreat() not here )
        for(int cnt=9, tmpx=restX-4*dx, tmpy=restY-4*dy;
                cnt>0;
                cnt--, tmpx+=dx, tmpy+=dy, restSquareMask <<=2){
            if( cnt==5 ) restSquareMask >>=2; //tmpx == x && tmpy == y, cnt=6 >>2 once, cnt=5 >>2 again
            if( brd->get(tmpx, tmpy) !=EMPTY ) continue;
            Threat *t = checkThreat(brd, turn, tmpx, tmpy, dx, dy, restSquareMask);
            if( t!=NULL ) threatList.push_back(t);
        }
    }

    sort( threatList.begin(), threatList.end(), threatCmp );
    return threatList;
}

// Finds the max threat
//   with (x,y), which is an empty/turn square, to be a gain square (for every direction)
// 尋找以(x,y)為gain square的所有方向的threats中 最大的那個
Threat* findMaxThreat(BoardData *brd,int turn,int x,int y)
{
    Threat *ans = NULL;
    for(int dir=0; dir<4 ; dir++){
        Threat *t = checkThreat(brd,turn,x,y,directions[dir][0],directions[dir][1],0b1111111111111111);
        if(t == NULL)
            continue;
        else if(ans == NULL || t->type > ans->type){
            delete ans;
            ans = t;
        }else delete t;
    }
    return ans;
}


Threat* findMaxThreat(BoardData *brd,int turn,bool gainSquareIsEmpty)
{
    Threat *ans = NULL;
    for(int i=0; i < brd->size(); i++)
        for(int j=0; j < brd->size(); j++){
            if( gainSquareIsEmpty ){
                if( brd->get(i, j) != EMPTY ) continue;
            }else {
                if( brd->get(i, j) != turn ) continue;
            }
            Threat *t = findMaxThreat(brd, turn, i, j);
            if(t == NULL)
                continue;
            else if(ans == NULL || t->type > ans->type){
                delete ans;
                ans = t;
            }else delete t;
        }
    return ans;
}


// returns [turn]'s all threats that > minThreat
// the threat list is sorted
// (minThreat could be NULL, meaning that no threats are to be sieved)
// gainSquareIsEmpty specifies that if gain square is empty or not
// (gainSquareIsEmpty==true => gain square is EMPTY => the next-step threats of [turn])
vector< Threat* >               // return a list of size 0 if no threats are found
findAllThreat(
        BoardData *brd,
        int turn,               // should be BLACK or WHITE
        bool gainSquareIsEmpty, // two choices: gain square empty or not
        Threat *minThreat)      // can be NULL: means no minimum search requirement
{
    if(turn != BLACK && turn != WHITE)
        err(true, "findAllThreat(): [turn] should be BLACK[%d] or WHITE[%d] but is [%d]\n", BLACK, WHITE, turn);
    if(brd == NULL)
        err(true, "findAllThreat(): [brd] shouldn't be NULL\n");

    vector< Threat* > list;
    int minThreatType = (minThreat == NULL) ? (0) : (minThreat->type / 10);
    for(int i=0; i < brd->size(); i++){
        for(int j=0; j < brd->size(); j++){
            if( gainSquareIsEmpty ){
                if( brd->get(i, j) != EMPTY ) continue;
            }else {
                if( brd->get(i, j) != turn ) continue;
            }
            for(int dir=0; dir<4; dir++){
                Threat *t = checkThreat(
                        brd, turn, i, j,
                        directions[dir][0], directions[dir][1],
                        0b1111111111111111);
                if( (t != NULL) && ((t->type/10) > (minThreatType)) ) list.push_back( t );
            }
        }
    }
    sort( list.begin(), list.end(), threatCmp );
    return list;
}

vector< vector<int> > g_threatTypeArr;
bool availableMoveCmp(pair<int,int> a, pair<int,int> b)
{
    if(g_threatTypeArr[a.first][a.second] != g_threatTypeArr[b.first][b.second])
        return g_threatTypeArr[a.first][a.second] <= g_threatTypeArr[b.first][b.second];
    else
        return (
            ((a.first)-7)*((a.first)-7)+((a.second)-7)*((a.second)-7) >
            ((b.first)-7)*((b.first)-7)+((b.second)-7)*((b.second)-7) );
}

vector< pair<int,int> > availableMove(BoardData *brd, int turn)
{
    vector< Threat* > opn = findAllThreat( brd, opposite(turn), false, NULL );

    /*****************************
     * 越好的選擇在list的越後面
     *****************************/
    set< pair<int,int> > moveSet;

    /*****************************
     * when opn != NULL
     * list = ( defensive moves ) + ( available attack moves )
     ****************************/
    if(opn.size()!=0){
        // the square to stop opponent's threat (only consider opponent's one max threat)
        for(int o=opn.size()-1; o>=0; o--){
            for(int i=0; i<opn[o]->costNum; i++)
                //list.push_back(make_pair( opn[o]->getCostX(i), opn[o]->getCostY(i) ) );
                moveSet.insert(make_pair( opn[o]->getCostX(i), opn[o]->getCostY(i) ) );
        }
        // the squares that can form bigger threats than opponent's
        vector< Threat* > my = findAllThreat(brd, turn, true, opn[0]);
        for(int i=my.size()-1; i>=0; i--){
            //list.push_back( make_pair( my[i]->getGainX(), my[i]->getGainY() ) );
            moveSet.insert( make_pair( my[i]->getGainX(), my[i]->getGainY() ) );
            delete my[i];
        }
        vector< pair<int,int> > list (moveSet.begin(), moveSet.end());
        return list;
    }
    /*****************************
     * when opn == NULL
     * list = ( empty squares with central-first-ordering ) + ( all attack moves )
     ****************************/
/*    vector< pair<int,int> > list;
    Threat *my = NULL;
    for(int i=0; i<brd->size(); i++)
        for(int j=0; j<brd->size(); j++){
            if(brd->get(i,j) != EMPTY) continue;
            my = findMaxThreat(brd, turn, i, j);
            if( my==NULL ) list.push_back(make_pair(i, j));
        }
    sort( list.begin(), list.end(), genChildrenListCmp );
    vector< Threat* > tmpList = findAllThreat(brd, turn, true, NULL);
    for(int i=tmpList.size()-1; i>=0; i--){
        list.push_back( make_pair( tmpList[i]->getGainX(), tmpList[i]->getGainY() ) );
        delete tmpList[i];
    }
    return list;*/
    g_threatTypeArr = vector< vector<int> > (brd->size(), vector<int>(brd->size()));
    vector< pair<int,int> > list;
    for(int i=0; i<brd->size(); i++)
        for(int j=0; j<brd->size(); j++){
            if(brd->get(i,j) != EMPTY) continue;
            list.push_back(make_pair(i, j));
            Threat *t = findMaxThreat(brd, turn, i,j);
            if(t == NULL) g_threatTypeArr[i][j] = -1;
            else g_threatTypeArr[i][j] = t->type;
        }
    sort( list.begin(), list.end(), availableMoveCmp );
    return list;
}

vector< pair<int,int> > attackMove(BoardData *brd, int turn)
{
    // 越好的選擇在list的越後面
    vector< pair<int,int> > list;

    Threat *opn = findMaxThreat( brd, opposite(turn), false );

    vector< Threat* > tmpList = findAllThreat(brd, turn, true, opn);
    for(int i=tmpList.size()-1; i>=0; i--){
        list.push_back( make_pair( tmpList[i]->getGainX(), tmpList[i]->getGainY() ) );
        delete tmpList[i];
    }
    return list;
}

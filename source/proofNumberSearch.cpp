#include "proofNumberSearch.h"
#include "paper.h"

//#define PNSTEST
#define PRINT_PNSEARCH_DETAIL

bool genChildrenListCmp(pair<int,int> a, pair<int,int> b)
{
    return (
            ((a.first)-7)*((a.first)-7)+((a.second)-7)*((a.second)-7) >
            ((b.first)-7)*((b.first)-7)+((b.second)-7)*((b.second)-7) );
}

void genChildrenList(pnsNode *n)
{
    for(int i=0; i< n->brd->size(); i++)
        for(int j=0; j< n->brd->size(); j++){
            if(n->brd->get(i, j)==EMPTY) n->virtualChildren.push_back(make_pair(i,j));
        }
    sort( n->virtualChildren.begin(), n->virtualChildren.end(), genChildrenListCmp );
}


pnsNode* updatePnDn(pnsNode *node, int pnBefore, int dnBefore, int rootTurn)
{
    if( node->parent==NULL ) return NULL;

    pnsNode *nodeParent = node->parent;
    // backup
    int nodeParentPnBefore = nodeParent->pn;
    int nodeParentDnBefore = nodeParent->dn;

    if( nodeParent->turn == rootTurn )
    {
        if( pnBefore != node->pn )
        {
            /******************************************
             * sort the actualChildren[] by pn again: pnMax---->pnMin
             ******************************************/
            // nodeParent[nodeParent.size()-1 = initial i] is node
            // node is the previous pnMin ( in the end of the actualChildren[] )
            int i;
            for(i=nodeParent->actualChildren.size()-1; i>0; i--)
                if(node->pn > nodeParent->actualChildren[i-1]->pn)
                    nodeParent->actualChildren[i] = nodeParent->actualChildren[i-1];
                else break;
            nodeParent->actualChildren[i] = node;
            /******************************************
             * max node =>  pn = min( all child.pn )
             ******************************************/
            // the last element in actualChildren[] has min pn
            nodeParent->pn = nodeParent->actualChildren[ nodeParent->actualChildren.size()-1 ]->pn;
            // there exists terminal node (unmalloc node)
            // terminal node's pn is always 1
            // (1 < nodeParent->pn) is for when (nodeParent->pn == 0)
            if( (nodeParent->virtualChildren.size() > 0) && (1 < nodeParent->pn) )
                nodeParent->pn = 1;
        }
        if( dnBefore != node->dn )
        {
            // max node => dn = sum( all child.dn )
            nodeParent->dn -= dnBefore;
            nodeParent->dn += node->dn;
        }
    }
    else if( nodeParent->turn == opposite(rootTurn) )
    {
        if( pnBefore != node->pn )
        {
            // min node => pn = sum( all child.pn )
            nodeParent->pn -= pnBefore;
            nodeParent->pn += node->pn;
        }
        if( dnBefore != node->dn )
        {
            /******************************************
             * sort the actualChildren[] by dn again: dnMax---->dnMin
             ******************************************/
            // nodeParent[nodeParent.size()-1 = initial i] is node
            // node is the previous dnMin ( in the end of the actualChildren[] )
            int i;
            for(i=nodeParent->actualChildren.size()-1; i>0; i--)
                if(node->dn > nodeParent->actualChildren[i-1]->dn)
                    nodeParent->actualChildren[i] = nodeParent->actualChildren[i-1];
                else break;
            nodeParent->actualChildren[i] = node;
            /******************************************
             * min node =>  dn = min( all child.dn )
             ******************************************/
            // the last element in actualChildren[] has min dn
            nodeParent->dn = nodeParent->actualChildren[ nodeParent->actualChildren.size()-1 ]->dn;
            // there exists terminal node (unmalloc node)
            // terminal node's dn is always 1
            // (1 < nodeParent->dn) is for when (nodeParent->dn == 0)
            if( (nodeParent->virtualChildren.size() > 0) && (1 < nodeParent->dn) )
                nodeParent->dn = 1;
        }
    }
    else
        err(true, "pnsSearch(): node color has wrong value.");


    /***************************************************
     * return most-proving-node
     ***************************************************/
    if( (nodeParent->pn == nodeParentPnBefore)&&(nodeParent->dn == nodeParentDnBefore) )
        // nodeParent's pn&dn is not changed => mpn is (under) nodeParent
        return nodeParent;
    else
        // mpn is changed, not in current tree
        return updatePnDn(nodeParent, nodeParentPnBefore, nodeParentDnBefore, rootTurn);
}

// return value is the most-proving-node
// ( return NULL if not in current tree )
int g_pnAgain=0;
pnsNode* proofNumberSearch(pnsNode *node, int rootTurn, bool runTSS, Paper *paper)
{
    if(node==NULL) err(true, "proofNumberSearch() parameter value error: [node] shouldn't be NULL.");

    /***************************************************
     * virtualChildren is empty : node is a leaf (a temp terminal node)
     ***************************************************/
    if(node->virtualChildren.size()==0 && node->actualChildren.size() ==0)
    {
        if(node->pn!=1 || node->dn!=1) err(true, "proofNumberSearch(): terminal pnsNode pn/dn value error");

        // different action varies by "runTSS"
        int winValue = NOWIN;
        if(runTSS){
            // run evaluation function, record the winning seq found by threatSpaceSearch
            winValue = threatSpaceSearch(node->brd, node->turn, node->winningSeq, paper);
            if(g_pnAgain==0 && winValue!=WIN && node->turn == opposite(rootTurn) ){
                pair<int, int> ans;
                g_pnAgain=1;
                int tmpWinValue = getTruthByPNS(node->brd, node->turn, opposite(node->turn), true, &ans);
                g_pnAgain=0;
                if(tmpWinValue==WIN) winValue = LOSE;
            }
        }else {
            // find all (turn)'s threats which are bigger than opn
            Threat *opn = findMaxThreat( node->brd, opposite(node->turn), false );
            vector< Threat* > my = findAllThreat(node->brd, node->turn, true, opn);
#ifdef PRINT_PNSEARCH_DETAIL
            safePrint(paper, ">>>>>>>>>>>>>>>>>>>\n");
            if(opn!=NULL) opn->printThreat(paper);
            if(my.size()!=0) my[0]->printThreat(paper);
            safePrint(paper, ">>>>>>>>>>>>>>>>>>>\n");
#endif
            if(my.size()!=0 && (my[0]->type==Five || my[0]->type==StrF) )winValue=WIN;
            else winValue=NOWIN;
        }

        if( winValue == WIN ){
            // find WIN => this node is a real terminal node
            if(node->turn == rootTurn){
                node->pn = 0;
                node->dn = PNS_INFINITY;
            }else{
                node->pn = PNS_INFINITY;
                node->dn = 0;
            }
#ifdef PNSTEST
            safePrint(paper, "=> WIN,%d,(%d,%d),<%d,%d>\n", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
            // pn/dn has changed,
            // update parents' pn/dn ( 1 = the original pn/dn value )
            // and return new most-proving-node
            return updatePnDn(node, 1, 1, rootTurn);;
        }else if(winValue == LOSE){
            // means that node->turn is lost
            if(node->turn == rootTurn){
                node->pn = PNS_INFINITY;
                node->dn = 0;
            }else{
                node->pn = 0;
                node->dn = PNS_INFINITY;
            }
            // pn/dn has changed,
            // update parents' pn/dn ( 1 = the original pn/dn value )
            // and return new most-proving-node
#ifdef PNSTEST
            printf("=> LOSE,%d,(%d,%d),<%d,%d>\n", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
            return updatePnDn(node, 1, 1, rootTurn);
        }else{
            // => this node is not a real terminal node => expand its virtualChildren
            node->virtualChildren = availableMove(node->brd, node->turn);
            /*******************************************************
             * for faster: rootTurn's cut
             * cut children to only squares who form threat
             * ****************************************************/
            /*
            if(node->turn == rootTurn){
                vector< pair<int,int> > threatList=attackMove(node->brd,node->turn);
                if(threatList.size()!=0) node->virtualChildren = threatList;
            }*/
            /*******************************************************/
            if(node->virtualChildren.size()==0){
                // means that node->turn has no move, is lost
                if(node->turn == rootTurn){
                    node->pn = PNS_INFINITY;
                    node->dn = 0;
                }else{
                    node->pn = 0;
                    node->dn = PNS_INFINITY;
                }
                // pn/dn has changed,
                // update parents' pn/dn ( 1 = the original pn/dn value )
                // and return new most-proving-node
#ifdef PNSTEST
                safePrint(paper, "=> LOSE,%d,(%d,%d),<%d,%d>\n", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
                return updatePnDn(node, 1, 1, rootTurn);
            }else if(node->turn == rootTurn){
                /****** max node ******/
                // pn = min( all child.pn ) = 1 == original node->pn
                // dn = sum( all child.dn )
                node->dn = node->virtualChildren.size();
#ifdef PNSTEST
                safePrint(paper, "=> leaf,%d,(%d,%d),<%d,%d>\n", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
                // both pn/dn hasn't changed, most-proving-node is still under this tree
                if( node->dn==1 ) return node;
                else {
                    // dn has changed, (pn hasn't)
                    // update parents' dn ( 1 = the original dn value )
                    // and return new most-proving-node
                    return updatePnDn(node, 1, 1, rootTurn);
                }
            }else{
                /****** min node ******/
                // dn = min( all child.dn ) = 1 == original node->dn
                // pn = sum( all child.pn )
                node->pn = node->virtualChildren.size();
#ifdef PNSTEST
                safePrint(paper, "=> leaf,%d,(%d,%d),<%d,%d>\n", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
                // both pn/dn hasn't changed, most-proving-node is still under this tree
                if( node->pn==1 ) return node;
                else {
                    // pn has changed,
                    // update parents' pn ( 1 = the original pn value )
                    // and return new most-proving-node
                    return updatePnDn(node, 1, 1, rootTurn);
                }
            }
        }
    }


    /***************************************************
     * node is an internal node
     ***************************************************/
    pnsNode *successor = NULL;
    if( node->actualChildren.size()!=0 )
        // the actualChildren in the end of actualChildren[] has min pn/dn
        successor = node->actualChildren[ node->actualChildren.size()-1 ];
    if( successor==NULL ||  //actualChildren.size()==0
            // in max node, the child who has min pn is the successor
            ( node->turn==rootTurn && successor->pn!=node->pn ) ||
            // in min node, the child who has min dn is the successor
            ( node->turn==opposite(rootTurn) && successor->dn!=node->dn ) )
    {
        if( node->virtualChildren.size() == 0 )
            err(true, "proofNumberSearch(): can't find any successor at the node");
        successor = new pnsNode(node, node->virtualChildren.back() );
        node->actualChildren.push_back(successor);
        node->virtualChildren.pop_back();
    }

#ifdef PNSTEST
    safePrint(paper, "=> %d,(%d,%d),<%d,%d> ", opposite(node->turn), node->x, node->y, node->pn, node->dn);
#endif
    return proofNumberSearch(successor, rootTurn, runTSS, paper);

}

void buildTreeWidgetItem(pnsNode *node, QTreeWidgetItem *rootItem)
{
    if(rootItem==NULL) err(false, "buildTreeWidget(): treeWidget shouldn't be NULL");
    if(node==NULL) err(false, "buildTreeWidget(): node shouldn't be NULL");

    QString str;
    str.sprintf("%s (%d,%d) [%d,%d] ",
                (( (opposite(node->turn))==BLACK )?"BLACK":"WHITE"),
                node->x, node->y, node->pn, node->dn);/*
                (node->pn >= PNS_INFINITY)?"INF":QString::number(node->pn),
                (node->dn >= PNS_INFINITY)?"INF":QString::number(node->dn) );*/
    rootItem->setText(0, str);

    QTreeWidgetItem *tItem;
    if(node->actualChildren.size()!=0) rootItem->setExpanded(true);
    for(int i=node->actualChildren.size()-1; i>=0; i--){
        tItem = new QTreeWidgetItem(rootItem);
        buildTreeWidgetItem(node->actualChildren[i], tItem);
    }
    if( node->virtualChildren.size()!=0 ){
        tItem = new QTreeWidgetItem(rootItem);
        str.sprintf("other unspanned %d nodes", node->virtualChildren.size());
        tItem->setText(0, str);

        QTreeWidgetItem *xItem = new QTreeWidgetItem(tItem);
        QString tmp;
        str.clear();
        for(int i=node->virtualChildren.size()-1; i>=0; i--){
            tmp.sprintf("(%d,%d) ", node->virtualChildren[i].first, node->virtualChildren[i].second);
            str.append(tmp);
        }
        xItem->setText(0, str);
    }
}

void printTreePnDn(pnsNode *node, Paper *paper)
{
    if(paper == NULL) return;
/*    safePrint(paper, " %d,%d/%d,%d/%d ", node->x, node->y, node->pn, node->dn, opposite(node->turn));
    safePrint(paper, "(");
    for(uint i=0;i<node->virtualChildren.size();i++)
        safePrint(paper, "/%d,%d/ ", node->virtualChildren[i].first, node->virtualChildren[i].second);
    for(uint i=0;i<node->actualChildren.size();i++)
        printTreePnDn(node->actualChildren[i], paper);
    safePrint(paper, ")");*/

    qDebug(" %d,%d/%d,%d/%d ", node->x, node->y, node->pn, node->dn, opposite(node->turn));
    qDebug("(");
    for(uint i=0;i<node->virtualChildren.size();i++)
        qDebug("/%d,%d/ ", node->virtualChildren[i].first, node->virtualChildren[i].second);
    for(uint i=0;i<node->actualChildren.size();i++)
        printTreePnDn(node->actualChildren[i], paper);
    qDebug(")");
}

void printWinningTree(pnsNode *node, Paper *paper)
{
    if(paper == NULL) return;
    safePrint(paper, "%d,%d ", node->x, node->y);
    safePrint(paper, "(");

    int flag = 0;
    for(uint i=0; i<node->actualChildren.size();i++)
        if(node->actualChildren[i]->pn==0){
            flag = 1;
            printWinningTree( node->actualChildren[i], paper);
        }
    if(flag == 0){
        if( node->actualChildren.size() == 0 ){
            safePrint(paper, "<< ");
            for(int i=0; i<node->winningSeq.size(); i++)
                safePrint(paper, "%d,%d ", node->winningSeq[i].first, node->winningSeq[i].second);
            safePrint(paper, ">>");
        }else err(false, "printWinningTree(): cant find the next winning step, should be an error in pnSearch");
    }
    safePrint(paper, ")");
}

#define maxLoopCnt 300
int
getTruthByPNS(
        BoardData *brd,
        int turn,
        int turnToBeProved,
        bool runTSS,          // if is true => run TSS => need ans
        pair<int,int> *ans,
        Paper *paper,
        pnsNode** rootNode)
{
    if(runTSS) safePrint(paper, "\n######################################\n");
    else safePrint(paper, "\n..............¡õpnSearch(%d,%d)...............\n",turn,turnToBeProved);

    pnsNode *root = new pnsNode(new BoardData(brd), turn);
    if(rootNode!=NULL) *rootNode = root;
    pnsNode *fakeRoot;
    if(turn==turnToBeProved)
        fakeRoot=root;
    else {
        fakeRoot = new pnsNode(new BoardData(brd), turnToBeProved);
        root->parent = fakeRoot;
        fakeRoot->actualChildren.push_back(root);
    }
    // most-proving-node
    pnsNode *mpn = root;
    int cnt=0;
    // (pn==0 || dn==0) means the result has been found
    while( root->pn!=0 && root->dn!=0 ){
        if( runTSS==false ) {
            if(cnt>maxLoopCnt){
                root->pn=PNS_INFINITY;
                root->dn=0;
                break;
            }else cnt++;
        }
        mpn = proofNumberSearch(mpn, turnToBeProved, runTSS, paper);
        //if( mpn==NULL ) mpn = root;

        mpn=root;
#ifdef PRINT_PNSEARCH_DETAIL
        printTreePnDn(root, paper);
        safePrint(paper, "\n");
#endif
    }

    // if tss is included => overall pns => need an answer
    if(runTSS && root->pn==0){
        // find the answer (one step)
        int flag = 0;
        for(uint i=0; i<root->actualChildren.size();i++)
            if(root->actualChildren[i]->pn==0){
                flag = 1;
                //ans = & (make_pair(root->actualChildren[i]->x, root->actualChildren[i]->y) );
                //ans = new pair<int,int>(make_pair(root->actualChildren[i]->x, root->actualChildren[i]->y) );
                //safePrint(paper, " (%d,%d)\n", ans->first, ans->second);
                if(ans==NULL) err(true, "in proofNumberSearch(), ans should not be NULL");
                ans->first = root->actualChildren[i]->x;
                ans->second = root->actualChildren[i]->y;
            }
        if(flag == 0){
            if( root->actualChildren.size() == 0 ){
                //ans = & (make_pair(root->winningSeq[0].first, root->winningSeq[0].second) );
                //ans = new pair<int,int>(make_pair(root->winningSeq[0].first, root->winningSeq[0].second) );
                //safePrint(paper, " (%d,%d)\n", ans->first, ans->second);
                if(ans==NULL) err(true, "in proofNumberSearch(), ans should not be NULL");
                if(root->winningSeq.size()==0) err(true, "getTruthByPNS(): root->winningSeq.Size==0, should be an error in pnSearch");
                ans->first = root->winningSeq[0].first;
                ans->second = root->winningSeq[0].second;
            }else err(false, "getTruthByPNS(): cant find the next winning step, should be an error in pnSearch");
        }
    }
    // print result
    //brd->printBoard();
    safePrint(paper, "turn = %d, pnSearchResult = %s \n", turn, (root->pn==0)? "WIN" : "LOSE");
    if(root->pn == 0) printWinningTree(root, paper); // if WIN, print win seq
    safePrint(paper, "\n");
    if(runTSS) safePrint(paper, "######################################\n");
    else safePrint(paper, "..............¡ôpnSearch...............\n");

    return ( (root->pn==0)? (WIN) : (NOWIN) );
}


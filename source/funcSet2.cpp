#include "funcSet2.h"
#include <QGridLayout>
#include "guiSettings.h"
#include "threatSpaceSearch.h"
#include "proofNumberSearch.h"
#include <stdlib.h>

FuncSet2::FuncSet2(QWidget *parent) :
    FuncSetAbstract(parent)
{
    //paper->setFont(QFont("Consolas"));
    button->setText("threatSpaceSearch()");
/*    tmp = NULL;
    a = new QTextEdit(this);
    b = new QTextEdit(this);
    a->setGeometry(
            POS_INTERACT_INPUTA_X, POS_INTERACT_INPUTA_Y,
            SIZE_INTERACT_INPUTA_W, SIZE_INTERACT_INPUTA_H);
    b->setGeometry(
            POS_INTERACT_INPUTB_X, POS_INTERACT_INPUTB_Y,
            SIZE_INTERACT_INPUTB_W, SIZE_INTERACT_INPUTB_H);
    button->setGeometry(
            POS_INTERACT_BUTTON_X, POS_INTERACT_BUTTON_Y,
            SIZE_INTERACT_BUTTON_W, SIZE_INTERACT_BUTTON_H);*/
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(turnButton, 0, 0);
    layout->addWidget(button, 0, 1);
    layout->addWidget(paper, 1, 0, 1, 2);
    this->setLayout(layout);
}

void FuncSet2::triggerFunc()
{
    if(brd == NULL) {
        safePrint(paper, "brd is NULL\n");
        return;
    }

    vector< pair<int,int> > ans;
    safePrint(paper, "Type\tGainSquare\tCostSquares\n");
    int winValue = threatSpaceSearch(brd, turn, ans, paper);
    if(paper == NULL) return;
    paper->print("\n%s %s\n",
                 (turn==BLACK)?"BLACK":"WHITE",
                 (winValue==WIN)?"WIN":"NOWIN");
    paper->print("threat seq = ");
    for(int i=0; i<ans.size(); i++)
        paper->print("(%d,%d) ", ans[i].first, ans[i].second);
    paper->print("\n\n");
    paper->changeColor();
    paper->moveCusorToTheEnd();
}

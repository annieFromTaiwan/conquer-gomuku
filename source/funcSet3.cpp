#include "funcSet3.h"
#include <QGridLayout>
#include "findThreat.h"

FuncSet3::FuncSet3(QWidget *parent) :
    FuncSetAbstract(parent)
{
    button->setText("availableMove()");

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(turnButton, 0, 0);
    layout->addWidget(button, 0, 1);
    layout->addWidget(paper, 1, 0, 1, 2);
    this->setLayout(layout);
}

void FuncSet3::triggerFunc()
{
    if(brd == NULL) {
        safePrint(paper, "brd is NULL\n");
        return;
    }

    vector< pair<int,int> > ans = availableMove(brd, turn);
    if(paper == NULL) return;
    paper->print("turn = %s\n", (turn==BLACK)?"BLACK":"WHITE");
    for(uint i=0; i<ans.size(); i++)
        paper->print("(%X,%X) ", ans[i].first, ans[i].second);
    paper->print("<br>");
    paper->changeColor();
    paper->moveCusorToTheEnd();
}

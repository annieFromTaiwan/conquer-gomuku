#include "funcSet4.h"
#include <QGridLayout>
#include "proofNumberSearch.h"
#include "guiSettings.h"

FuncSet4::FuncSet4(QWidget *parent) :
    FuncSetAbstract(parent)
{
    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    readme1 = new QLabel(TradChineseCodec->toUnicode("目前輪到"));
    QGridLayout *leftUp = new QGridLayout();
    leftUp->addWidget(readme1, 0, 0);
    leftUp->addWidget(turnButton, 0, 1);

    readme2a = new QLabel(TradChineseCodec->toUnicode("確認"));
    readme2b = new QLabel(TradChineseCodec->toUnicode("能贏否"));
    turnTobeProvedButton = new QPushButton();
    turnTobeProved = BLACK;
    turnTobeProvedButton->setText(TradChineseCodec->toUnicode("黑方"));
    QObject::connect(turnTobeProvedButton, SIGNAL(clicked()), this, SLOT(changeTurnTobeProved()));
    QObject::connect(this, SIGNAL(turnChanged(int)), SLOT(changeTurnTobeProved(int)) );
    QGridLayout *leftDn = new QGridLayout();
    leftDn->addWidget(readme2a, 0, 0);
    leftDn->addWidget(turnTobeProvedButton, 0, 1);
    leftDn->addWidget(readme2b, 0, 2);

    button->setText("proofNumberSearch()");
    button->setMinimumWidth(button->text().length()*8);
    QGridLayout *layout = new QGridLayout();
    layout->addLayout(leftUp, 0, 0);
    layout->addLayout(leftDn, 1, 0);
    layout->addWidget(button, 0, 1, 2, 1);
    layout->addWidget(paper,  2, 0, 1, 2);

    layout->addWidget(&pnSearchTree, 3, 0, 1, 2);
    pnSearchTree.setMinimumHeight(400);
    pnSearchTree.setHeaderLabel("nextTurn (x,y) [pn,dn]");
    root = NULL;

    this->setLayout(layout);
}

void FuncSet4::triggerFunc()
{    
    if(brd == NULL) {
        safePrint(paper, "brd is NULL\n");
        return;
    }

    if(root!=NULL) root->~QTreeWidgetItem();
    root = new QTreeWidgetItem();
    pnSearchTree.addTopLevelItem(root);

    pair<int,int> *ans = new pair<int,int>();
    pnsNode *rootNode;
    int winValue = getTruthByPNS(brd, turn, turnTobeProved, true, ans, NULL, &rootNode);
    buildTreeWidgetItem(rootNode, root);
    //printTreePnDn(rootNode, paper);

    if(paper == NULL) return;
    paper->print("winValue = %s\n", (winValue==WIN)?"WIN":"NOWIN");
    paper->print("(x,y) = (%d,%d)\n", ans->first, ans->second);
    paper->changeColor();
    paper->moveCusorToTheEnd();
}

void FuncSet4::changeTurnTobeProved()
{
    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    turnTobeProved = opposite(turnTobeProved);
    turnTobeProvedButton->setText( TradChineseCodec->toUnicode(
                    (turnTobeProved==BLACK) ? "黑方" : "白方" ) );
}

void FuncSet4::changeTurnTobeProved(int t)
{
    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    if(t!=BLACK && t!=WHITE)return;
    turnTobeProved = t;
    turnTobeProvedButton->setText( TradChineseCodec->toUnicode(
                    (turnTobeProved==BLACK) ? "黑方" : "白方" ) );
}

#include "funcSet1.h"
#include <QGridLayout>
#include "guiSettings.h"
#include "findThreat.h"
#include "threat.h"

FuncSet1::FuncSet1(QWidget *parent) :
    FuncSetAbstract(parent)
{
    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    readme1 = new QLabel(TradChineseCodec->toUnicode("所有"));
    readme2 = new QLabel(TradChineseCodec->toUnicode("的攻擊"));

    emptyGainSquareButton = new QPushButton();
    gainSquareIsEmpty = true;
    emptyGainSquareButton->setText(TradChineseCodec->toUnicode("下一步"));
    QObject::connect(emptyGainSquareButton, SIGNAL(clicked()), this, SLOT(changeEmptyGainSquare()));

    button->setText("findThreat()");
    turnButton->setMinimumWidth(button->text().length()*4);
    emptyGainSquareButton->setMinimumWidth(button->text().length()*4);
    button->setMinimumWidth(button->text().length()*8);
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(readme1, 0, 0);
    layout->addWidget(turnButton, 0, 1);
    layout->addWidget(emptyGainSquareButton, 0, 2);
    layout->addWidget(readme2, 0, 3);
    layout->addWidget(button, 0, 4);
    layout->addWidget(paper, 1, 0, 1, 5);
    this->setLayout(layout);


    //this->setStyleSheet("background-color:black;");
}

void FuncSet1::triggerFunc()
{
    if(brd == NULL) {
        safePrint(paper, "brd is NULL\n");
        return;
    }
    vector<Threat*> threats = findAllThreat(brd, turn, gainSquareIsEmpty, NULL);

    if(threats.size() == 0){
        safePrint(paper, "%s has no %s threats in the board\n",
                  (turn==BLACK)?"BLACK":"WHITE",
          (gainSquareIsEmpty==true)?"FUTURE":"CURRENT" );
        safePrint(paper, "================================\n");
    }else{
        safePrint(paper, "Type\tGainSquare\tCostSquares\n");
        for(uint i=0; i<threats.size(); i++)
            threats[i]->printThreat(paper);
        safePrint(paper, "================================\n");
    }
}

void FuncSet1::changeEmptyGainSquare()
{
    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    gainSquareIsEmpty = !gainSquareIsEmpty;
    emptyGainSquareButton->setText( TradChineseCodec->toUnicode(
                    (gainSquareIsEmpty==true) ? "下一步" : "已存在" ) );
}


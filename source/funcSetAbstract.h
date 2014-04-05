#ifndef INTERACT_H
#define INTERACT_H

#include <QWidget>
#include "boardData.h"
#include <QPushButton>
#include "Paper.h"
#include "guiSettings.h"
#include <QTextCodec>

class FuncSetAbstract : public QWidget
{
    Q_OBJECT
public:
    explicit FuncSetAbstract(QWidget *parent = 0) : QWidget(parent){
        QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");

        brd = NULL;

        turnButton = new QPushButton(this);
        turn = BLACK;
        turnButton->setText(TradChineseCodec->toUnicode("黑方"));
        QObject::connect(turnButton, SIGNAL(clicked()), this, SLOT(changeTurn()));

        button = new QPushButton(this);
        QObject::connect(button, SIGNAL(clicked()), this, SLOT(triggerFunc()));

        paper = new Paper(this);

        QPalette p(palette());
        p.setColor(QPalette::Background, QColor("#D1E9E9"));
        this->setPalette(p);
        this->setAutoFillBackground(true);
    }
    void setBrd(BoardData *x){ brd = x; }

protected:
    //input
    BoardData *brd;
    QPushButton *turnButton;
    int turn;
    //the very button to run function
    QPushButton *button;
    //output
    Paper *paper;

signals:
    void turnChanged(int newTurn);

public slots:
    virtual void triggerFunc() = 0;
    void changeTurn(){
        QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");

        turn = opposite(turn);
        turnButton->setText(TradChineseCodec->toUnicode( (turn==BLACK) ? "黑方" : "白方" ));
        emit turnChanged(turn);
    }
    void changeTurn(int turnToChange){
        QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");

        if(turnToChange!=BLACK && turnToChange!=WHITE) return;
        turn = turnToChange;
        turnButton->setText(TradChineseCodec->toUnicode( (turn==BLACK) ? "黑方" : "白方" ));
        emit turnChanged(turn);
    }

};

#endif // INTERACT_H

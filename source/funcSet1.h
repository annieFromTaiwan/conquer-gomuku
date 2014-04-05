#ifndef FUNCSET1_H
#define FUNCSET1_H

#include "funcSetAbstract.h"
#include <QPushButton>
#include <QLabel>
#include "paper.h"

class FuncSet1 : public FuncSetAbstract
{
    Q_OBJECT
public:
    explicit FuncSet1(QWidget *parent = 0);
private:
    QPushButton *emptyGainSquareButton;
    int gainSquareIsEmpty;
    QLabel *readme1;
    QLabel *readme2;
public slots:
    void triggerFunc();
    void changeEmptyGainSquare();
};

#endif // FUNCSET1_H

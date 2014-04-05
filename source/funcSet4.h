#ifndef FUNCSET4_H
#define FUNCSET4_H

#include "funcSetAbstract.h"
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>

class FuncSet4 : public FuncSetAbstract
{
    Q_OBJECT
public:
    explicit FuncSet4(QWidget *parent = 0);

private:
    QLabel *readme1;
    QLabel *readme2a;
    QLabel *readme2b;
    QPushButton *turnTobeProvedButton;
    QTreeWidget pnSearchTree;
    QTreeWidgetItem *root;
    int turnTobeProved;

public slots:
    void triggerFunc();
    void changeTurnTobeProved();
    void changeTurnTobeProved(int t);
};

#endif // FUNCSET4_H

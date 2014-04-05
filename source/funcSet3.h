#ifndef FUNCSET3_H
#define FUNCSET3_H

#include "funcSetAbstract.h"

class FuncSet3 : public FuncSetAbstract
{
    Q_OBJECT
public:
    explicit FuncSet3(QWidget *parent = 0);

private:

public slots:
    void triggerFunc();
};

#endif // FUNCSET3_H

#ifndef FUNCSET2_H
#define FUNCSET2_H

#include "funcSetAbstract.h"

class FuncSet2 : public FuncSetAbstract
{
    Q_OBJECT
public:
    explicit FuncSet2(QWidget *parent = 0);

private:

public slots:
    void triggerFunc();
};

#endif // FUNCSET2_H

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QObject>
#include <QLabel>
#include "boardData.h"
#include <QMouseEvent>

class Chessboard : public QLabel
{
    Q_OBJECT
public:
    explicit Chessboard(QLabel *parent = 0);
    BoardData boardData;

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPixmap stoneImg[4];
    QLabel *stonesPlane;

signals:
    void changeFuncsTurn(int turn);

public slots:
    void cleanChessboard();
    void readFromTxt();

};

#endif // CHESSBOARD_H

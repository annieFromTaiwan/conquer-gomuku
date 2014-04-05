#include <QApplication>
#include <QLabel>
#include <QGridLayout>
#include <QPixmap>
#include "guiSettings.h"
#include "chessboard.h"
#include "boardData.h"
#include "funcSet1.h"
#include "funcSet2.h"
#include "funcSet3.h"
#include "funcSet4.h"
#include <QTextCodec>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QPalette>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPalette p;
    p.setColor(QPalette::Background, QColor("#97CBFF"));
    a.setPalette(p);
    //a.setAutoFillBackground(true);

    QLabel *window = new QLabel();
    window->setWindowTitle(TEXT_WINDOW_TITLE);

    QLabel *horCoordinate = new QLabel();
    QPixmap hCoord(PATH_HCOORD_IMG);
    horCoordinate->setPixmap(hCoord);
    horCoordinate->setMinimumSize(SIZE_COORDINATE_L, SIZE_COORDINATE_W);

    QLabel *verCoordinate = new QLabel();
    QPixmap vCoord(PATH_VCOORD_IMG);
    verCoordinate->setPixmap(vCoord);
    verCoordinate->setMinimumSize(SIZE_COORDINATE_W, SIZE_COORDINATE_L);

    Chessboard *chessboard = new Chessboard();
    QPixmap img(PATH_CHESSBORAD_IMG);
    chessboard->setPixmap(img.scaled(SIZE_CHESSBOARD, SIZE_CHESSBOARD));
    chessboard->setMinimumSize(SIZE_CHESSBOARD, SIZE_CHESSBOARD);

    QTextCodec *TradChineseCodec = QTextCodec::codecForName("Big5-ETen");
    QLabel *chessboardReadme = new QLabel(TradChineseCodec->toUnicode(
                                              "<center> <b>黑方</b> 滑鼠左鍵 |<b>  白方</b> 滑鼠右鍵 |<b>  清空</b> 滑鼠中鍵 </center>"));
    chessboardReadme->setAlignment(Qt::AlignHCenter);
    QLabel *funcReadme = new QLabel(TradChineseCodec->toUnicode(" <br> <br> <br> <br> <br><br> <br> <br> <br>"));

    QPushButton *clearBoard = new QPushButton(TradChineseCodec->toUnicode("清空棋盤"));
    QObject::connect(clearBoard, SIGNAL(clicked()), chessboard, SLOT(cleanChessboard()));
    QPushButton *readBoardData = new QPushButton(TradChineseCodec->toUnicode("讀取已存棋盤"));
    QObject::connect(readBoardData, SIGNAL(clicked()), chessboard, SLOT(readFromTxt()));
    QGridLayout *buttonLayout = new QGridLayout();
    buttonLayout->addWidget(readBoardData, 0, 0);
    buttonLayout->addWidget(clearBoard, 0, 1);

    QGridLayout *wholeChessboard = new QGridLayout;
    wholeChessboard->addWidget(horCoordinate, 0, 1);
    wholeChessboard->addWidget(verCoordinate, 1, 0);
    wholeChessboard->addWidget(chessboard, 1, 1);
    wholeChessboard->addWidget(chessboardReadme, 2, 0, 1, 2);
    wholeChessboard->addLayout(buttonLayout, 3, 0, 1, 2);
    wholeChessboard->addWidget(funcReadme, 4, 0, 1, 2);
    wholeChessboard->setMargin(20);


    FuncSet1 *findThreat = new FuncSet1();
    findThreat->setMinimumSize(SIZE_FUNC_W, SIZE_FUNC_H2);
    findThreat->setBrd(&(chessboard->boardData));

    FuncSet2 *threatSpaceSearch = new FuncSet2();
    threatSpaceSearch->setMinimumSize(SIZE_FUNC_W, SIZE_FUNC_H1);
    threatSpaceSearch->setBrd(&(chessboard->boardData));

    FuncSet3 *availableMove = new FuncSet3();
    availableMove->setMinimumSize(SIZE_FUNC_W, SIZE_FUNC_H2);
    availableMove->setBrd(&(chessboard->boardData));

    FuncSet4 *proofNumberSearch = new FuncSet4();
    proofNumberSearch->setMinimumSize(SIZE_FUNC_W, SIZE_FUNC_H1);
    proofNumberSearch->setBrd(&(chessboard->boardData));


    QGridLayout *wholeApp = new QGridLayout();
    wholeApp->addLayout(wholeChessboard,   0, 0, 3, 1);
    wholeApp->addWidget(availableMove,     0, 1);
    wholeApp->addWidget(findThreat,        1, 1);
    wholeApp->addWidget(threatSpaceSearch, 2, 1);
    wholeApp->addWidget(proofNumberSearch, 0, 2, 3, 1);
    wholeApp->setSpacing(15);


    QObject::connect(chessboard, SIGNAL(changeFuncsTurn(int)), availableMove,     SLOT(changeTurn(int)));
    QObject::connect(chessboard, SIGNAL(changeFuncsTurn(int)), findThreat,        SLOT(changeTurn(int)));
    QObject::connect(chessboard, SIGNAL(changeFuncsTurn(int)), threatSpaceSearch, SLOT(changeTurn(int)));
    QObject::connect(chessboard, SIGNAL(changeFuncsTurn(int)), proofNumberSearch, SLOT(changeTurn(int)));

    //wholeChessboard->addWidget(test, 4, 0, 1, 2);
    /*Paper *ya = new Paper();
    wholeApp->addWidget(ya, 2,2);

    BoardData orig;
    orig.printBoard(ya);
    orig.set(0,0,BLACK);
    orig.set(0,1,WHITE);
    orig.printBoard(ya);
    BoardData *tmp = new BoardData(&orig);
    tmp->set(1,1,BLACK);
    tmp->printBoard(ya);
    orig.printBoard(ya);*/

    window->setLayout(wholeApp);


    // 把其他method也寫進去

    // 以上先以"測試階段"的感覺去寫
    // last step: 整理code 還有output

    // threatspaceSearch 23

    window->show();
    return a.exec();
}

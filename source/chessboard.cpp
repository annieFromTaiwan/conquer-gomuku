#include "chessboard.h"
#include "guiSettings.h"
#include "err.h"

Chessboard::Chessboard(QLabel *parent) : QLabel(parent){
    stoneImg[BLACK].load(PATH_BLACKSTONE_IMG);
    stoneImg[WHITE].load(PATH_WHITESTONE_IMG);
    stoneImg[EMPTY].load(PATH_EMPTYSTONE_IMG);
}

void Chessboard::mouseReleaseEvent(QMouseEvent *event) {
    int turn;
    if(event->button() == Qt::LeftButton) turn = BLACK;
    else if(event->button() == Qt::RightButton) turn = WHITE;
    else if(event->button() == Qt::MidButton) turn = EMPTY;
    else return;

    unsigned char stoneType = boardData.set(event->y() / SIZE_STONE, event->x() / SIZE_STONE, turn);
    if(stoneType != turn){
        err(false, "mouseReleaseEvent() boardData.set error: click at [%d, %d]\n", event->x(), event->y());
        return;
    }

    QLabel *frame = new QLabel(this);
    frame->setPixmap( stoneImg[stoneType].scaled(SIZE_STONE, SIZE_STONE) );
    frame->setGeometry( (event->x() / SIZE_STONE) * SIZE_STONE,
                        (event->y() / SIZE_STONE) * SIZE_STONE,
                        SIZE_STONE, SIZE_STONE);
    frame->show();
}

void Chessboard::cleanChessboard(){
    QList<QObject*> cList = this->children();
    for(int i=0; i<cList.length(); i++)
        cList[i]->deleteLater();
    for(int i=0; i<boardData.size(); i++)
        for(int j=0; j<boardData.size(); j++)
            if( boardData.set(i, j, EMPTY) != EMPTY ) err(true, "boardData set error");
}

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
void Chessboard::readFromTxt()
{
    QString input;

    static QString defaultPath = "..\\";
    QString fileName = QFileDialog::getOpenFileName(this, "Open Text File", defaultPath, "Text Files (*.txt);;All files (*.*)");
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly) != true) return;
    QTextStream stream(&file);
    input = stream.readAll();
    file.close();
    defaultPath = fileName;

    cleanChessboard();
    int cnt=0;
    int i;
    for(i=0; i<input.size(); i++){
        char tmp = input.at(i).toAscii();
        if(tmp==' '||tmp=='\n'||tmp==13)continue;
        int x = cnt/boardData.size();
        int y = cnt%boardData.size();
        int rv = boardData.set(x, y, tmp-'0');
        // rv=return value, error judgement
        if(rv== -1)
            err(true,"character should be %d or %d or %d",EMPTY,BLACK,WHITE);
        if(rv!=tmp-'0')
            err(false,"map[%d][%d] wasn't changed.", cnt/boardData.size(), cnt%boardData.size());

        QLabel *frame = new QLabel(this);
        frame->setPixmap( stoneImg[tmp-'0'].scaled(SIZE_STONE, SIZE_STONE) );
        frame->setGeometry(y*SIZE_STONE, x*SIZE_STONE, SIZE_STONE, SIZE_STONE);
        frame->show();

        cnt++;
        if(cnt >= boardData.size()*boardData.size()) break;
    }
    while(1){
        char tmp = input.at(++i).toAscii();
        if(tmp==BLACK+'0' || tmp==WHITE+'0'){
            emit changeFuncsTurn(tmp-'0');
            break;
        }
        if(i+1==input.size())break;
    }

    //QString t = input.at(i)
}

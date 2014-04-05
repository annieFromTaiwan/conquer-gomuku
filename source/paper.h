#ifndef PAPER_H
#define PAPER_H

#include <QObject>
#include <QTextEdit>

class Paper : public QTextEdit
{
    Q_OBJECT
public:
    Paper(QWidget *parent = 0);
    void print(const char *msg, ...);
    void print(QString msg);
    void changeColor(){
        colorSwitch = !colorSwitch;
    }

signals:
    void printSignal(QString);

public slots:
    void addColorAndInsert(QString text);
    void moveCusorToTheEnd(){
        QTextCursor cursor = this->textCursor();
        cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        this->setTextCursor(cursor);
    }

private:
    bool colorSwitch;
    //QString t1 = QString("<pre><font style=background:azure face=Consolas>");
    QString start[2];
    QString end;
};

void safePrint(Paper *paper, QString msg);
void safePrint(Paper *paper, const char *msg, ...);

#endif // PAPER_H

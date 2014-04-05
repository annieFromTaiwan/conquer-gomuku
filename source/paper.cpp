#include "paper.h"
#include <QString>
#include <cstdarg>
#include <stdio.h>

Paper::Paper(QWidget *parent) : QTextEdit(parent)
{
    colorSwitch = true;
    start[0] = QString("<pre><font face=Dotum color=midnightblue>");
    start[1] = QString("<pre><font face=Dotum color=maroon>");
    end = QString("</font></pre>");

    QObject::connect(this, SIGNAL(printSignal(QString)), SLOT(addColorAndInsert(QString)));
    //QObject::connect(this, SIGNAL(textChanged()), SLOT(moveCusorToTheEnd()));
}

void Paper::print(const char *msg, ...)
{
    if(msg==NULL) return;
    va_list args;
    va_start (args, msg);
    QString buffer;
    buffer.vsprintf (msg, args);
    va_end (args);

    emit printSignal(buffer);
}

void Paper::print(QString msg)
{
    emit printSignal(msg);
}

void Paper::addColorAndInsert(QString text){
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    this->setTextCursor(cursor);

    text.replace("\n","<br>");
    cursor.insertHtml(start[colorSwitch? 0 : 1] + text + end);
    //cursor.insertText(text);
}

void safePrint(Paper *paper, QString msg)
{
    if(paper == NULL) return;
    paper->print(msg);
}

void safePrint(Paper *paper, const char *msg, ...)
{
    if(paper == NULL) return;

    if(msg == NULL) return;
    va_list args;
    va_start (args, msg);
    QString buffer;
    buffer.vsprintf (msg, args);
    va_end (args);

    paper->print(buffer);
}

#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include "err.h"
#include <vector>
#include "paper.h"

#define SIZE_DEFAULT 15

// changes the NOTICE place if the value of BLACK & WHITE changes
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define BOUND 3

#define WIN 1
#define NOWIN 0
#define LOSE 2

using namespace std;

inline int opposite(int turn)
{
    if(turn != BLACK && turn != WHITE)
        err(true, "opposite(): [turn] should be WHITE or BLACK\n");
    turn ^= 0b11; // NOTICE: changes simultaneousely with the BLACK & WHITE value
    return turn;
}

class BoardData : public QObject
{
    Q_OBJECT
    public:
        BoardData(){
            boardSize = SIZE_DEFAULT;
            twoDimArr = vector< vector<unsigned char> >(boardSize,vector<unsigned char>(boardSize,EMPTY));
        }
        BoardData(int size){
            boardSize=size;
            twoDimArr = vector< vector<unsigned char> >(boardSize,vector<unsigned char>(boardSize,EMPTY));
        }
        BoardData(vector< vector<unsigned char> > arr){
            boardSize=arr.size();
            twoDimArr=arr;
        }
        BoardData(BoardData *b){
            boardSize = b->twoDimArr.size();
            twoDimArr = vector< vector<unsigned char> > (b->twoDimArr);
        }

        int size(){
            return boardSize;
        }

        unsigned char get(int i, int j){
            if(i<0 || i>=boardSize || j<0 || j>=boardSize) return BOUND;
            else return twoDimArr[i][j];
        }

        void printBoard(Paper *paper){
            if(paper == NULL)return;
            paper->print("\n");
            int i,j;
            for(paper->print("  "), j=0; j<boardSize; j++)
                paper->print("%X ",j);
            paper->print("\n");
            for(i=0; i<boardSize; i++){
                paper->print("%X ",i);
                for(j=0; j<boardSize; j++)
                    if( get(i,j)==EMPTY ) paper->print("  ");
                    else paper->print( "%d ", get(i,j) );
                paper->print("\n");
            }
        }

    private:
        int boardSize;
        vector< vector<unsigned char> > twoDimArr;

signals:

        public slots:
            unsigned char set(int i,int j,unsigned char value){
                if(value!=EMPTY && value!=BLACK && value!=WHITE)
                    err(true, "BoardData.set(): [value] should be WHITE or BLACK or EMPTY\n");
                else if(i<0 || i>=boardSize || j<0 || j>=boardSize)
                    return BOUND;
                else{
                    twoDimArr[i][j]=value;
                    return value;
                }
            }

};

#endif // BOARD_H

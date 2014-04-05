#ifndef THREAT_H
#define THREAT_H

#include <vector>
#include <cstdarg>
#include "boardData.h"
#include "paper.h"

// *****************************************************
// threat type
#define Five 59

#define StrF 49     // straight four (empty at two sides)
#define Four 42     // with one side enemy and another empty (1 cost square)
#define BrkF 41     // broken four (1 cost square)

#define StrT 32     // stright three (2 kinds- 2 or 3 cost squares)
#define BrkT 31     // broken three (3 cost squares)
// *****************************************************

inline char* getTypeName(int no){
    switch(no){
        case Five: return (char*)"Five";
        case StrF: return (char*)"StrF";
        case Four: return (char*)"Four";
        case BrkF: return (char*)"BrkF";
        case StrT: return (char*)"StrT";
        case BrkT: return (char*)"BrkT";
        default: return NULL;
    }
}

class BasicThreat {
    public:
        BasicThreat(unsigned short int m, unsigned short int s, int t, int cn, ...){
            mask=m;
            seqb=s;
            type=t;
            costNum=cn;

            va_list argList;
            va_start(argList, cn);
            for(int i=0; i<costNum; i++) cost[i]=va_arg(argList,int);
            va_end(argList);
        }

        int type;
        unsigned short int mask;
        unsigned short int seqb;  // binary sequence

        int costNum;
        int cost[3];
};

class Threat {
    public:
        // gain square (x,y)
        // direction (dx,dy)
        // BasicThreatList[no]
        Threat(int x,int y,int dx,int dy,const BasicThreat* basicThreat){
            gain = x & 0b00001111 ;
            gain <<= 4 ;
            gain += y & 0b00001111 ;

            costNum = basicThreat->costNum;
            for(int i=0; i<costNum; i++){
                int costX = x + basicThreat->cost[i]*dx ;
                int costY = y + basicThreat->cost[i]*dy ;
                cost[i] = costX & 0b00001111 ;
                cost[i] <<= 4 ;
                cost[i] += costY & 0b00001111 ;
            }

            type = basicThreat->type;
        }

        int type;
        int costNum;
        int getGainX(){ return int((gain & 0b11110000)>>4); }
        int getGainY(){ return int(gain & 0b00001111); }
        int getCostX(int no){ return int((cost[no] & 0b11110000)>>4); }
        int getCostY(int no){ return int(cost[no] & 0b00001111); }

        bool biggerThan(Threat *t){
            if(t==NULL) return true;
            else return (type/10 > t->type/10);
        }

        void printThreat(Paper *paper){
            if(paper == NULL) return;
            paper->print("%s\t",getTypeName(type) );
            paper->print("%02X\t",gain);
            for(int i=0; i<costNum; i++) paper->print("%02X,",cost[i]);
            if(type==Five || type==StrF) paper->print("\t<= WIN");
            paper->print("\n");
        }
    private:
        unsigned char gain;
        unsigned char cost[3];
};

#define BasicThreatListLen 44
// to be used in findThreat()
const BasicThreat *BasicThreatList(int x);

#endif // THREAT_H

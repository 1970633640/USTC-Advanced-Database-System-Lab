#ifndef BMGR_H_INCLUDED
#define BMGR_H_INCLUDED


#define DEFBUFSIZE 1024

#include "DSMgr.h"
using namespace std;



bFrame buf[DEFBUFSIZE]; // or the size that the user defined by the input parameter

struct NewPage
{
    int page_id;
    int frame_id;
};

struct BCB
{
    BCB();
    int page_id;
    int frame_id;
    int count;
    int dirty;
    BCB * next;
};

class BMgr
{
public:
    BMgr();
// Interface functions
    int FixPage(int page_id, int prot);
    NewPage FixNewPage(); //题目写错了！
    int UnfixPage(int page_id);
    int NumFreeFrames();
// Internal Functions
    int SelectVictim();
    int Hash(int page_id);
    void RemoveBCB(BCB * ptr, int page_id);
    void RemoveLRUEle(int frid);
    void SetDirty(int frame_id);
    void UnsetDirty(int frame_id);
    void WriteDirtys();
    void PrintFrame(int frame_id);
//自定义函数
    DSMgr ds;
private:
// Hash Table
    int ftop[DEFBUFSIZE];
    BCB* ptof[DEFBUFSIZE];
};


#endif // BMGR_H_INCLUDED

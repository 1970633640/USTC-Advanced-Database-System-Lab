#ifndef BM_H_INCLUDED
#define BM_H_INCLUDED

#include "dsm.h"
#include <list>
#include <vector>
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
    NewPage FixNewPage(); //题目写错了！  而且这个函数根本用不到！
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
    DSMgr ds; //Data Storage Manager of this Buffer Manager

private:

    // Hash Table   page_id:0-49999    frame_id of buf:0-4095
    int ftop[DEFBUFSIZE]; //frame_id to page_id, default value -1
    BCB *ptof[DEFBUFSIZE];  //page_id to BCB chain head (contains BCB->frame_id) 例如实体文件page的4096*1、2、3都进了缓存 这些的ptof是个链表 即开链法解决哈希冲突

    list<int> lru; //lru列表 内容为frame_id list头表示最近使用 尾部表示最近最久未使用
    bFrame* buf[DEFBUFSIZE]; // or the size that the user defined by the input parameter --X
    int newBCB(int page_id); //return frame_id
};


#endif // BM_H_INCLUDED

#include "BM.h"
#include "DSM.h"
#include <iostream>
using namespace std;
BCB::BCB()
{

}

BM::BM()
{

}

int BM::FixPage(int page_id, int prot)
{
    int frame_id = -1; //返回值
    if(prot != 0)
    {
        cout << "警告！prot参数没有任何用处！" << endl;
    }
    BCB* bcb = ptof[Hash(page_id)];
    while(bcb->next != NULL && bcb->page_id != page_id)
    {
        bcb = bcb->next;
    }
    if(bcb != NULL) //准备读写的页面已经在缓存中，应该更新lru表 缓存命中
    {
        frame_id = bcb->frame_id;
        lru.remove(frame_id);
        lru.push_front(frame_id);
    }
    else //准备读写的页面不在缓存中，需要读取、建立BCB、检查并处理LRU是否需要淘汰、更新LRU
    {
        BCB* bcb = new BCB();
        if(lru.size() < DEFBUFSIZE) //lru没满
        {
            bcb->count = 0;
            bcb->dirty = 0;
            bcb->page_id = page_id;
            bcb->next = NULL;
            bool suc = false;
            for(int i = 0; i > DEFBUFSIZE; i++)
                if(ftop[i] == -1)
                {
                    ftop[i] = page_id;
                    bcb->frame_id = i;
                    frame_id = i;
                    suc = true;
                    break;
                }
            if(suc == false)cout << "错误！LRU和ftop关系维护错误！！！！！" << endl;
            lru.push_front(frame_id);
        }
        else //LRU满了 需要淘汰
        {
            frame_id = SelectVictim(); //此时已经被淘汰、释放
            lru.remove(frame_id);
            bcb->count = 0;
            bcb->dirty = 0;
            bcb->page_id = page_id;
            bcb->next = NULL;
            bcb->frame_id = frame_id;
            lru.push_front(frame_id);
        }
        //新建的bcb需要连接到哈希表开链上
        BCB* head = ptof[Hash(bcb->page_id)];
        if(head == NULL)
        {
            ptof[Hash(bcb->page_id)] = bcb;
        }
        else
        {
            while(head->next != NULL)head = head->next;
            head->next = bcb;
        }
    }
    bcb->count = bcb->count + 1;
    return frame_id;
}

NewPage BM::FixNewPage() //题目写错了！  而且这个函数根本用不到！
{
    cout << "警告！正在使用本实验不需要的函数！" << endl;
}

int BM::UnfixPage(int page_id)
{
    BCB* bcb = ptof[Hash(page_id)];
    while(bcb->next != NULL && bcb->page_id != page_id)
    {
        bcb = bcb->next;
    }
    if(bcb == NULL)
    {
        cout << "缓存错误！尝试删除不在缓存中的页面缓存！" << endl;
    }
    bcb->count = bcb->count - 1;
    return bcb->frame_id;
}

int BM::NumFreeFrames()
{
    int answer = 0;
    for (int i = 0; i < DEFBUFSIZE; i++)
    {
        if(ftop[i] == -1)
        {
            answer += 1;
        }
    }
    return answer;
}

// Internal Functions
int BM::SelectVictim()
{
    int answer;
    BCB* bcb = NULL;
    list<int>::iterator it = lru.end(); //指向尾部后面一个元素
    while(1)
    {
        if(it == lru.begin())break;
        it--;
        bcb = ptof[Hash(ftop[*it])];
        while(bcb->frame_id != *it && bcb->next != NULL)  bcb = bcb->next;
        if(bcb->frame_id == *it && bcb->count <= 0)break;
    }
    if(bcb->frame_id != *it || bcb->count > 0)
    {
        cout << "错误！Buffer中所有项目都被占用，无法释放！" << endl;
        return -1;
    }
    else
    {
        answer = bcb->frame_id;
        if(bcb->dirty == true)
        {
            ds.WritePage(bcb->page_id, buf[answer]);
        }
        RemoveBCB(bcb, bcb->page_id);
        return answer;
    }
}

int BM::Hash(int page_id)
{
    return page_id % DEFBUFSIZE;
}

void BM::RemoveBCB(BCB * ptr, int page_id) //删除链表元素
{
    BCB* head = ptof[Hash(ptr->page_id)];
    ftop[ptr->frame_id] = -1; //释放哈希表
    if(head == ptr) //要删除的是链表头
    {
        ptof[Hash(ptr->page_id)] = ptr->next;
        delete ptr;
    }
    else //要删除的不是头
    {
        while(head->next != ptr)head = head->next;
        head->next = ptr->next;
        delete ptr;
    }
}

void BM::RemoveLRUEle(int frid)
{
    lru.remove(frid);
}

void BM::SetDirty(int frame_id)
{
    BCB* bcb = ptof[Hash(ftop[frame_id])];
    while(bcb->next != NULL && bcb->frame_id != frame_id)bcb = bcb->next;
    if(bcb->frame_id != frame_id)cout << "错误！缓存内容异常！！！！！！！！！！！！";
    else bcb->dirty = 1;
}

void BM::UnsetDirty(int frame_id)
{
    BCB* bcb = ptof[Hash(ftop[frame_id])];
    while(bcb->next != NULL && bcb->frame_id != frame_id)bcb = bcb->next;
    if(bcb->frame_id != frame_id)cout << "错误！缓存内容异常！！！！！！！！！！！！";
    else bcb->dirty = 0;
}

void BM::WriteDirtys()
{
    list<int>::iterator it;
    for(it = lru.begin(); it != lru.end(); it++)
    {
        BCB* bcb = ptof[Hash(ftop[*it])];
        while(bcb->next != NULL && bcb->frame_id != *it)bcb = bcb->next;
        if(bcb->frame_id != *it)
        {
            cout << "错误！缓存内容异常！！！！！！！！！！！！无法判断是否应该写回";
        }
        else
        {
            if(bcb->dirty == 1)
            {
                ds.WritePage(bcb->page_id, buf[bcb->frame_id]);
            }
        }
    }
}
void BM::PrintFrame(int frame_id)
{
    cout << "frame_id:" << frame_id << " \nvalue:" << buf[frame_id].field << endl;
}

void BM::InitBuf(int buf_size)
{
    for(int i = 0; i < buf_size; i++)
    {
        ftop[i] = -1;
        ptof[i] = NULL;
        memset(buf[i].field, 0, FRAMESIZE); //非必须
    }
}

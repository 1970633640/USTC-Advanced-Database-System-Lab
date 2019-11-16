#include "bm.h"
#include <stdlib.h>

int hit = 0;
int miss = 0;


BCB::BCB(): page_id(-1), frame_id(-1), count(0), dirty(0), next(NULL)
{
}

BMgr::BMgr()
{
    for(int i = 0; i < DEFBUFSIZE; i++)
    {
        ftop[i] = -1;
        ptof[i] = NULL;
        buf[i] = (bFrame*)malloc( sizeof(bFrame));
    }
}

int BMgr::newBCB(int page_id)
{
    int frame_id = -1;
    BCB* bcb = new BCB();
    if(lru.size() < DEFBUFSIZE) //lru没满
    {

    }
    else //LRU满了 需要淘汰
    {
        frame_id = SelectVictim(); //此时已经被淘汰、释放
        lru.remove(frame_id);
    }
    //找到ftop的空余并使用  但是需要之后的操作建立BCB->frame关系
    bcb->page_id = page_id;
    bool suc = false;
    for(int i = 0; i < DEFBUFSIZE; i++)
        if(ftop[i] == -1)
        {
            ftop[i] = page_id;
            bcb->frame_id = i;
            frame_id = i;
            suc = true;
            break;
        }
    if(suc == false)cout << "错误！LRU和ftop关系维护错误！！！！！" << endl;
    //新建的bcb需要连接到ptof哈希表开链上  找到BCB才获取frame 从而实现ptof
    BCB* head = ptof[Hash(bcb->page_id)];
    if(head == NULL) //此处是空 直接指向自身
    {
        ptof[Hash(bcb->page_id)] = bcb;
    }
    else //已有 最后一项的next指向自身
    {
        while(head->next != NULL)head = head->next;
        head->next = bcb;
    }
    bcb->count = bcb->count + 1;

    lru.push_front(frame_id);
    return frame_id;
}

int BMgr::FixPage(int page_id, int prot)
{
    int frame_id = -1; //返回值
    if (page_id >= 50000 || page_id<0)
    {
        cout << "警告！！测试数据超出允许范围！ " <<page_id<< endl;
    }
    if(prot != 0)
    {
        cout << "警告！prot参数没有任何用处！" << endl;
    }
    BCB* bcb = ptof[Hash(page_id)];
    while(bcb != NULL && bcb->next != NULL && bcb->page_id != page_id)
    {
        bcb = bcb->next;
    }
    if(bcb != NULL && bcb->page_id == page_id) //准备读写的页面已经在缓存中，应该更新lru表 缓存命中
    {
        hit += 1;
        frame_id = bcb->frame_id;
        lru.remove(frame_id);
        lru.push_front(frame_id);
        bcb->count = bcb->count + 1;
    }
    else //准备读写的页面不在缓存中，需要读取、建立BCB、检查并处理LRU是否需要淘汰、更新LRU
    {
        miss += 1;
        frame_id = newBCB(page_id);
        memcpy(buf[frame_id], (ds.ReadPage(page_id)).field, sizeof(*(buf[frame_id])));
    }

    return frame_id;
}

NewPage BMgr::FixNewPage() //题目写错了！
{
    NewPage newpage;
    int i=0;
    for(i = 0; i < ds.GetNumPages(); i++)
    {
        if(ds.GetUse(i) == 0)break;
    }
    if(i == ds.GetNumPages()) //所有page都被使用 需要增加dbf文件大小
    {
        ds.IncNumPages();
        ds.SetUse(i, 1);
    }
    newpage.page_id = i;
    newpage.frame_id = FixPage(i, 0);
    return newpage;
}

int BMgr::UnfixPage(int page_id)
{
    BCB* bcb = ptof[Hash(page_id)];
    while(bcb->next != NULL && bcb->page_id != page_id)
    {
        bcb = bcb->next;
    }
    if(bcb == NULL)
    {
        cout << "缓存错误！尝试释放不在缓存中的页面！" << endl;
    }
    bcb->count = bcb->count - 1;
    return bcb->frame_id;
}

int BMgr::NumFreeFrames()
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
int BMgr::SelectVictim()
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
            ds.WritePage(bcb->page_id, *(buf[answer]));
        }
        RemoveBCB(bcb, bcb->page_id);
        return answer;
    }
}

int BMgr::Hash(int page_id)
{
    return page_id % DEFBUFSIZE;
}

void BMgr::RemoveBCB(BCB * ptr, int page_id) //删除链表元素
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

void BMgr::RemoveLRUEle(int frid)
{
    lru.remove(frid);
}

void BMgr::SetDirty(int frame_id)
{
    BCB* bcb = ptof[Hash(ftop[frame_id])];
    while(bcb->next != NULL && bcb->frame_id != frame_id)bcb = bcb->next;
    if(bcb->frame_id != frame_id)cout << "错误！缓存内容异常！！！！！！！！！！！！";
    else bcb->dirty = 1;
}

void BMgr::UnsetDirty(int frame_id)
{
    BCB* bcb = ptof[Hash(ftop[frame_id])];
    while(bcb->next != NULL && bcb->frame_id != frame_id)bcb = bcb->next;
    if(bcb->frame_id != frame_id)cout << "错误！缓存内容异常！！！！！！！！！！！！";
    else bcb->dirty = 0;
}

void BMgr::WriteDirtys()
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
                ds.WritePage(bcb->page_id, *(buf[bcb->frame_id]));
            }
        }
    }
}
void BMgr::PrintFrame(int frame_id)
{
    cout << "frame_id:" << frame_id << " \nvalue:" << (*buf[frame_id]).field << endl;
}



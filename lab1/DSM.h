#ifndef DSM_H_INCLUDED
#define DSM_H_INCLUDED

#define DEFBUFSIZE 1
#define FRAMESIZE 4096
#define MAXPAGES 50000
#include <string.h>
#include <string>
#include <cstdio>
#include <iostream>

using namespace std;

struct bFrame
{
    char field[FRAMESIZE];  //题目写错了！typo
};


class DSMgr
{
public:
    int OpenFile(string filename);
    int CloseFile();
    bFrame ReadPage(int page_id);
    int WritePage(int frame_id, bFrame frm);
    int Seek(int offset, int pos);
    FILE * GetFile();
    void IncNumPages();
    int GetNumPages();
    void SetUse(int index, int use_bit);
    int GetUse(int index);
    //自定义函数
    void InitFile(string filename);

private:
    FILE *currFile;
    int numPages;  //页数 就是50000  实验中应该不会变
    int pages[MAXPAGES]; //就是初始化那里写了50000个1的地方 表示对应的page是否被使用  实验中应该都是1
    //自定义数据
    int PagesStart;//Page内容开始的字节数  应该对齐4096 从页数、50000个1之后开始
};

#endif // DSM_H_INCLUDED

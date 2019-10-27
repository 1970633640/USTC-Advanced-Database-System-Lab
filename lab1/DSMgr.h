#ifndef DSMGR_H_INCLUDED
#define DSMGR_H_INCLUDED

#define FRAMESIZE 4096
#define MAXPAGES 50000
#include <string.h>
#include <string>
#include <cstdio>
using namespace std;

struct bFrame
{
    char field [FRAMESIZE];  //题目写错了！
};

class DSMgr
{
public:
    DSMgr();
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
    void InitFile(string filename,int page_count);
private:
    FILE *currFile;
    int numPages;
    int pages[MAXPAGES];
};

#endif // DSMGR_H_INCLUDED

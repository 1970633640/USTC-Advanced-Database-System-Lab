#include "DSMgr.h"

using namespace std;

int DSMgr::OpenFile(string filename);
int DSMgr::CloseFile();
bFrame DSMgr::ReadPage(int page_id);
int DSMgr::WritePage(int frame_id, bFrame frm)
{
Seek(PagesStart,frame_id*FRAMESIZE)
fwrite()
}

int DSMgr::Seek(int offset, int pos)
{
    fseek(currFile,offset+pos,SEEK_SET)
}

FILE * DSMgr::GetFile()
{
    return currFile;
}

void DSMgr::IncNumPages()  //实验中用不到的功能
{
    numPages++;
}

int DSMgr::GetNumPages()
{
    return numPages;
}

void DSMgr::SetUse(int index, int use_bit)
{
    pages[index]=use_bit
}

int DSMgr::GetUse(int index)
{
    return pages[index];
}

//自定义函数
void DSMgr::InitFile(string filename,int page_count)
{
    FILE* file = fopen(filename,"w");
    char usage='1';
    fwrite(page_count,4,1,file) //写入int类型的50000 占4个字节
    fwrite(usage,1,page_count,file) //写50000个1字节的'1'
    fseek(file,int((page_count*1+1*4-1)/FRAMESIZE)*FRAMESIZE,SEEK_SET) //从第一页开始写
    char zero='\0';
    fwrite(zero,1,page_count*FRAMESIZE,file);
    fclose(file);
}

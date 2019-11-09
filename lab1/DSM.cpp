#include "dsm.h"




int DSMgr::OpenFile(string filename)
{
    cout<<"打开dbf文件"<<endl;
    currFile = fopen(filename.c_str(), "r+");
    fread(&numPages, 4, 1, currFile);
    cout << "文件中的page数:" << numPages << endl;
    PagesStart = int((MAXPAGES * 1 + 1 * 4 - 1) / FRAMESIZE + 1) * FRAMESIZE;
    cout << "文件中的page指针们从" << PagesStart << "字节处开始" << endl;
    char pagesbyte[MAXPAGES];
    memset(pagesbyte, 0, numPages);
    fread(pagesbyte, 1, numPages, currFile);
    for(int i = 0; i < numPages; i++)
    {
        if(pagesbyte[i] == '0')
            pages[i] = 0;
        else if(pagesbyte[i] == '1')
            pages[i] = 1;
        else
            cout << "读取错误：dbf文件异常！ 忽略" << endl;
    }
    return 1;
}

int DSMgr::CloseFile() //关闭时才写入page使用情况
{
    Seek(0, 0); //页数在开头
    fwrite(&numPages, 4, 1, currFile); //写入int类型的50000 占4个字节
    //int -> char
    char pagesbyte[MAXPAGES];
    memset(pagesbyte, 0, numPages);
    for(int i = 0; i < numPages; i++)
    {
        if(pages[i] == 0)
            pagesbyte[i] = '0';
        else
            pagesbyte[i] = '1';
    }
    //---------------
    fwrite(pagesbyte, numPages, 1, currFile);
    return fclose(currFile);
}

bFrame DSMgr::ReadPage(int page_id)
{
    bFrame bf;
    Seek(PagesStart, page_id * 4);
    int p;
    fread(&p, 4, 1, currFile);
    Seek(PagesStart, p);
    fread(bf.field, FRAMESIZE, 1, currFile);
    return bf;
}

int DSMgr::WritePage(int page_id, bFrame frm) //写页时不涉及写入页使用情况
{
    Seek(PagesStart, page_id * 4);
    int p;
    fread(&p, 4, 1, currFile);
    Seek(PagesStart, p);
    fwrite(frm.field, FRAMESIZE, 1, currFile);
    return FRAMESIZE;
}

int DSMgr::Seek(int offset, int pos)
{
    return fseek(currFile, offset + pos, SEEK_SET);
}

FILE * DSMgr::GetFile()
{
    return currFile;
}

void DSMgr::IncNumPages()
{
    if(numPages >= 53248)cout << "警告！超出实验允许的范围，dbf文件将损坏！！！" << endl;
    numPages++;
}

int DSMgr::GetNumPages()
{
    return numPages;
}

void DSMgr::SetUse(int index, int use_bit)
{
    if(pages[index]==0 && use_bit==1) //新开辟
    {
        int p=PagesStart+((MAXPAGES*4-1)/4096+1)*4096+4096*index;
        Seek(PagesStart,index*8);
        fwrite(&p, 4, 1, currFile); //写入指针
    }
    pages[index] = use_bit;
}

int DSMgr::GetUse(int index)
{
    return pages[index];
}

//自定义函数
void DSMgr::InitFile(string filename)
{
    FILE* file = fopen(filename.c_str(), "w");
    char usage[1] = {'0'};
    int page_count=0;
    fwrite(&page_count, 4, 1, file); //写入int类型的0 占4个字节
    for(int i = 0; i < MAXPAGES; i++)
        fwrite(usage, 1, 1, file); //写50000个1字节的'0'
    fclose(file);
}

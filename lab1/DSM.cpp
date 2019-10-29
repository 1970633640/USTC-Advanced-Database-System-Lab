#include "dsm.h"




int DSMgr::OpenFile(string filename)
{
    currFile = fopen(filename.c_str(), "r+");
    fread(&numPages, 4, 1, currFile);
    cout<<"文件中的page数:"<<numPages<<endl;
    PagesStart = int((numPages * 1 + 1 * 4 - 1) / FRAMESIZE + 1) * FRAMESIZE;
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
    Seek(PagesStart, page_id * FRAMESIZE);
    fread(bf.field, FRAMESIZE, 1, currFile);
    return bf;
}

int DSMgr::WritePage(int page_id, bFrame frm) //写页时不涉及写入页使用情况
{
    Seek(PagesStart, page_id * FRAMESIZE);
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
    pages[index] = use_bit;
}

int DSMgr::GetUse(int index)
{
    return pages[index];
}

//自定义函数
void DSMgr::InitFile(string filename, int page_count)
{
    FILE* file = fopen(filename.c_str(), "w");
    char usage[1] = {'1'};
    fwrite(&page_count, 4, 1, file); //写入int类型的50000 占4个字节
    for(int i=0;i<page_count;i++)
    fwrite(usage, 1, 1, file); //写50000个1字节的'1'
    fseek(file, int((page_count * 1 + 1 * 4 - 1) / FRAMESIZE + 1)*FRAMESIZE, SEEK_SET); //从第一页开始写
    char zero[FRAMESIZE] = {'\0'};
    for(int i=0;i<page_count;i++)
        fwrite(zero, FRAMESIZE, 1, file);
    fclose(file);
}

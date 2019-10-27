#include "DSMgr.h"
using namespace std;

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
void InitFile(string filename,int page_count)
{
FILE* file = fopen(filename,"w");
char usage='1';
fwrite(page_count,4,1,file) //写入int类型的50000 占4个字节
fwrite(usage,1,page_count,file) //写50000个1字节的'1'
fseek(file,,SEEK_SET) //从第一页开始写
}

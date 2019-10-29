#include "bm.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

extern int hit;
extern int miss;
extern int read;
extern int write;

int main()
{

    cout << "创建空dbf文件" << endl;
    FILE* file = fopen("data.dbf","w");
    fclose(file);

    cout << "创建dbf文件完毕" << endl;

   BMgr buffer_manager;
    buffer_manager.ds.InitFile("data.dbf", 50000);
    cout << "DSMgr初始化dbf文件完毕\n开始测试" << endl;

    buffer_manager.ds.OpenFile("data.dbf");
    FILE* file2 = fopen("data-5w-50w-zipf.txt", "r");
    int operation;
    int page;
    int frame_id;
    int i=0;
    while(fscanf(file2, "%d,%d", &operation, &page) != EOF)
    {
        i++;
        frame_id = buffer_manager.FixPage(page, 0);
        if(operation == 1)
        {
            //if write, set frame to dirty
            buffer_manager.SetDirty(frame_id);
        }
        buffer_manager.UnfixPage(page);
    }
    buffer_manager.WriteDirtys();
    buffer_manager.ds.CloseFile();
    cout<<"total tests:"<<i<<"\nhit: "<<hit<<" miss:"<<miss<<" read:"<<read<< " write:"<<write<<endl;
    return 0;
}


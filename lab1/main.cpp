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
    FILE* file = fopen("data.dbf", "w");
    fclose(file);

    cout << "创建dbf文件完毕" << endl;

    BMgr buffer_manager;
    BMgr buffer_manager_init; //初始化用的
    buffer_manager_init.ds.InitFile("data.dbf");
    buffer_manager_init.ds.OpenFile("data.dbf");
    for (int i=0;i<50000;i++)
    {
        NewPage newpage;
        newpage=buffer_manager_init.FixNewPage();
        buffer_manager_init.UnfixPage(newpage.page_id);
    }
    buffer_manager_init.ds.CloseFile();
    cout << "hit: " << hit << " miss:" << miss << endl;
    cout << "DSMgr初始化dbf文件完毕\n\n开始测试" << endl;

    hit=0;miss=0;read=0;write=0;
    buffer_manager.ds.OpenFile("data.dbf");
    FILE* file2 = fopen("data-5w-50w-zipf.txt", "r");
    int operation;
    int page;
    int frame_id;
    int i = 0;
    while(fscanf(file2, "%d,%d", &operation, &page) != EOF)
    {
        i++;
        page-=1;
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
    cout << "total tests:" << i << "\nhit: " << hit << " miss:" << miss << " read:" << read << " write:" << write << endl;
    return 0;
}


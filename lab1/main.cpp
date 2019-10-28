#include <iostream>
#include <cstdio>
#include "BM.h"

using namespace std;

int main()
{
    return 0;
    return 0;
    return 0;
    cout << "创建空dbf文件" << endl;
    FILE* file = fopen("data.dbf", "w");
    fclose(file);
    cout << "创建dbf文件完毕" << endl;
    BM buffer_manager;
    buffer_manager.ds.InitFile("data.dbf", 50000);
    cout << "DSM初始化dbf文件完毕" << endl;
    buffer_manager.ds.OpenFile("data.dbf");
    FILE* file2 = fopen("data-5w-50w-zipf.txt", "r");
    int operation;
    int page;
    int frame_id;
    while(fscanf(file2, "%d,%d", &operation, &page) != EOF)
    {
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
    return 0;
}

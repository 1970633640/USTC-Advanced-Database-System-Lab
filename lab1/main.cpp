#include <iostream>
#include <cstdio>
#include "BMgr.h"

using namespace std;

int main()
{
    cout << "创建空dbf文件" << endl;
    FILE* file=fopen("data.dbf","w");
    fclose(file);
    cout << "创建dbf文件完毕" << endl;
    BMgr buffer_manager;
    buffer_manager.ds.InitFile("data.dbf",50000)
    cout << "DSMgr初始化dbf文件完毕" << endl;
    buffer_manager.ds.OpenFile("data.dbf");
    return 0;
}

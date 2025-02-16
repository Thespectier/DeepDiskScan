#ifndef TEST_TWO_H
#define TEST_TWO_H
#include <windows.h>
#include <stdio.h>
//得到格式化修改时间
int getrighttime(FILETIME ftLastWriteTime,char* timestring)
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&ftLastWriteTime,&systemTime);
    wchar_t filetime[20];
    wsprintfW(filetime, L"%04d-%02d-%02d %02d:%02d:%02d",
              systemTime.wYear, systemTime.wMonth, systemTime.wDay,
              systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    wcstombs(timestring, filetime, sizeof(filetime));
    return 1;
}
//
int writefile(char* filename,FILETIME ftlastWriteTime,long filesize,long filenum)
{
    FILE *file;
    int num=0;
    char timestring[128];
    switch(filenum/60000)
    {
    case 0:
        file = fopen("FILEsql_1.sql","a+");
        num = 1;
        break;
    case 1:
        file = fopen("FILEsql_2.sql","a+");
        num = 2;
        break;
    case 2:
        file = fopen("FILEsql_3.sql","a+");
        num = 3;
        break;
    case 3:
        file = fopen("FILEsql_4.sql","a+");
        num = 4;
        break;
    case 4:
        file = fopen("FILEsql_5.sql","a+");
        num = 5;
        break;
    default:
        printf("file number is too much\n");\
            break;
    }
    getrighttime(ftlastWriteTime,timestring);
    fprintf(file,"insert into file_table values");
    fprintf(file,"(\"%s\",\"%s\",%ld)",filename,timestring,filesize);
    fprintf(file,";\n");
    fclose(file);
    return num;
}
//
int writedirectory(char* directoryname,FILETIME ftlastWriteTime,long dirnum)
{
    FILE *file;
    int num=0;
    char timestring[128];
    switch(dirnum/60000)
    {
    case 0:
        file = fopen("DIRsql_1.sql","a+");
        num = 1;
        break;
    case 1:
        file = fopen("DIRsql_2.sql","a+");
        num = 2;
        break;
    case 2:
        file = fopen("DIRsql_3.sql","a+");
        num = 3;
        break;
    case 3:
        file = fopen("DIRsql_4.sql","a+");
        num = 4;
        break;
    case 4:
        file = fopen("DIRsql_5.sql","a+");
        num = 5;
        break;
    default:
        printf("file number is too much\n");\
            break;
    }
    getrighttime(ftlastWriteTime,timestring);
    fprintf(file,"insert into directory_table values");
    fprintf(file,"(\"%s\",\"%s\")",directoryname,timestring);
    fprintf(file,";\n");
    fclose(file);
    return num;
}

#endif // TEST_TWO_H

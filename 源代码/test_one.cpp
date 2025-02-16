#include<windows.h>
#include<stdio.h>
#include"test_one.h"
#include<stdio.h>
#include<string.h>
#include<mbstring.h>
// ... 获取主窗口对象的代码

//初始化队列
LinkQueue InitQueue(){
    LinkQueue Q;
    Q.front = Q.rear = (QueuePtr)malloc(LENG);
    Q.front->next = NULL;
    return Q;
}

// 将结点入队列
int EnQueue(LinkQueue *Q, char* szFind,int nowdepth){
    Qnode *p;
    p = (Qnode *)malloc(LENG);
    if (!p) {
        printf("ERROR");
        return 0;
    }
    strcpy(p->dirmaxname,szFind);
    p->depth = nowdepth;
    p->next = NULL;
    (*Q).rear->next =  p;
    (*Q).rear = p;
    return 1;
};

// 将结点出队列
int DelQueue(LinkQueue *Q, char* szFind,int* nowdepth){
    Qnode *p;
    if ((*Q).front==(*Q).rear){
        return 0;
    }
    p = (*Q).front->next;
    strcpy(szFind,p->dirmaxname);
    *nowdepth = p->depth;
    (*Q).front->next  = (*Q).front->next->next ;
    if ((*Q).rear==p){
        (*Q).rear = (*Q).front;
    }
    free(p);
    return 1;
}

int T_EnQueue(LinkQueue *Q,Treeptr tnode ,int nowdepth){
    Qnode *p;
    p = (Qnode *)malloc(LENG);
    if (!p) {
        printf("ERROR");
        return 0;
    }
    p->tnode = tnode;
    p->depth = nowdepth;
    p->next = NULL;
    (*Q).rear->next =  p;
    (*Q).rear = p;
    return 1;
};

int T_DelQueue(LinkQueue *Q,Treeptr* tnode,int* nowdepth){
    Qnode *p;
    if ((*Q).front==(*Q).rear){
        return 0;
    }
    p = (*Q).front->next;
    *tnode = p->tnode;
    *nowdepth = p->depth;
    (*Q).front->next  = (*Q).front->next->next ;
    if ((*Q).rear==p){
        (*Q).rear = (*Q).front;
    }
    free(p);
    return 1;
}

//给定带全路径名在目录树中找到结点
Treeptr findnode(char* maxname,Treeptr head)
{
    char Tname[300];
    char Pname[300];
    char* str1;
    char* str2;
    int num1=0;
    int num2=0;
    const char delimiter[2]="\\";
    Treeptr Pnode;
    if(strncmp(maxname,head->maxname,strlen(head->maxname))!=0)
    {
        printf("该目录或文件不存在于目录树中！\n");
        return NULL;
    }
    strcpy_s(Tname,maxname);
    strcpy_s(Pname,head->maxname);
    str2 = strtok(Pname,delimiter);
    num2++;
    while(str2!=NULL)
    {
        num2++;
        str2 = strtok(NULL,delimiter);
    }
    str1 = strtok(Tname,delimiter);
    num1++;
    while(num1!=num2)
    {
        num1++;
        str1 = strtok(NULL,delimiter);
    }
    if(str1==NULL)
    {
        return head;
    }

    Pnode = head;
    while(str1!=NULL)
    {
        Pnode = Pnode->kid;
        if(Pnode==NULL)
        {
            //printf("该目录or文件不存在于目录树中！\n");
            return NULL;
        }
        while(strcmp(Pnode->name,str1)!=0)
        {
            Pnode = Pnode->brother;
            if(Pnode==NULL)
            {
                //printf("该目录or文件不存在于目录树中！\n");
                return NULL;
            }
        }
        str1 = strtok(NULL,delimiter);
    }
    return Pnode;
}

//生成目录树目录结点
Treeptr createDIRnode(char* Dmaxname,char* Dname,FILETIME ftlastWriteTime,Treeptr parent)
{
    Treeptr Dnode;
    Treeptr Pnode;
    Dnode = (Treeptr)malloc(sizeof(Tnode));
    strcpy_s(Dnode->maxname,Dmaxname);
    strcpy_s(Dnode->name,Dname);
    getrighttime(ftlastWriteTime,Dnode->time);
    Dnode->flag = 1;
    Dnode->brother = NULL;
    Dnode->kid = NULL;
    Dnode->Information = NULL;
    if(parent->kid==NULL)
    {
        parent->kid = Dnode;
        Dnode->lastptr = parent;
    }
    else
    {
        Pnode = parent->kid;
        while(Pnode->brother!=NULL)
        {
            Pnode = Pnode->brother;
        }
        Pnode->brother = Dnode;
        Dnode->lastptr = Pnode;
    }
    return Dnode;
}
//生成目录树文件结点
Treeptr createFILEnode(char* Fmaxname,WIN32_FIND_DATA* FindFileDate,Treeptr parent)
{
    Treeptr Fnode;
    Treeptr Pnode;
    Fnode = (Treeptr)malloc(sizeof(Tnode));
    strcpy_s(Fnode->maxname,Fmaxname);
    char buffer[520];
    wcstombs(buffer,FindFileDate->cFileName, MAX_PATH);
    strcpy_s(Fnode->name,buffer);
    Fnode->fltwritetime = FindFileDate->ftLastWriteTime;
    getrighttime(FindFileDate->ftLastWriteTime,Fnode->time);
    Fnode->flag = 2;
    Fnode->size = FindFileDate->nFileSizeLow;
    Fnode->brother = NULL;
    Fnode->kid = NULL;
    Fnode->Information = NULL;
    if(parent->kid==NULL)
    {
        parent->kid = Fnode;
        Fnode->lastptr = parent;
    }
    else
    {
        Pnode = parent->kid;
        while(Pnode->brother!=NULL)
        {
            Pnode = Pnode->brother;
        }
        Pnode->brother = Fnode;
        Fnode->lastptr = Pnode;
    }
    return Fnode;
}
//遍历计算目录树深度
int getTreeDepth(Treeptr head)
{
    LinkQueue TREEqueue= InitQueue();
    Treeptr Pnode;
    Treeptr Tnode;
    int nowdepth=1;
    int maxdepth=1;
    Pnode = head->kid;
    while(1)
    {
        while(Pnode!=NULL)
        {
            if(Pnode->flag==1)
            {
                nowdepth++;
                T_EnQueue(&TREEqueue,Pnode,nowdepth);
                Pnode = Pnode->brother;
            }
            else
            {
                nowdepth++;
                Pnode = Pnode->brother;
            }
            if(nowdepth>maxdepth)
            {
                maxdepth = nowdepth;
            }
        }
        if(T_DelQueue(&TREEqueue,&Pnode,&nowdepth)==0)
        {
            printf("目录树遍历完毕!\n");
            break;
        }
        else
        {
            Pnode = Pnode->kid;
        }
    }
    return maxdepth;
}
//进行时间转换
int getrighttime(FILETIME ftLastWriteTime,char* timestring)
{
    SYSTEMTIME systemTime;
    SYSTEMTIME UTCTime;
    FileTimeToSystemTime(&ftLastWriteTime,&UTCTime);
    // 获取本地时区信息
    TIME_ZONE_INFORMATION timeZoneInformation;
    GetTimeZoneInformation(&timeZoneInformation);
    // 将 UTC 时间转换为本地时间
    SystemTimeToTzSpecificLocalTime(&timeZoneInformation, &UTCTime,&systemTime);

    wchar_t filetime[20];
    wsprintfW(filetime, L"%04d-%02d-%02d %02d:%02d:%02d",
              systemTime.wYear, systemTime.wMonth, systemTime.wDay,
              systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
    wcstombs(timestring, filetime, sizeof(filetime));
    return 1;
}
//将文件信息写入sql文件
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
//将目录信息写入sql文件
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

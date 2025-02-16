#ifndef TEST_ONE_H
#define TEST_ONE_H
#include<windows.h>
#include"mainwindow.h"
typedef struct Tnode{
    char maxname[300];      //带全路径名称
    char name[300];         //单文件or目录名称
    int flag;               //flag==1为目录，flag==2为文件
    char time[128];         //修改时间
    FILETIME fltwritetime;    //存放FILETIME类型时间
    long size;               //大小，单位为字节
    struct Tnode *kid;        //孩子结点指针
    struct Tnode *brother;    //兄弟结点指针
    struct Tnode *lastptr;     //指向 指向本结点的结点 的指针
    struct Dnode *Information; //目录信息结点指针
}Tnode,*Treeptr;      //目录树结点结构体

typedef struct Dnode{
    long nowsize;             //存放现在的文件大小
    long nownumber;           //存放现在的文件数量
    long originsize;           //存放原始文件大小
    long originnumber;         //存放原始文件数量
    Treeptr nowFirstFile;       //指向现在修改时间最早的文件结点
    Treeptr nowLatestFile;      //指向现在修改时间最晚的文件结点
    Treeptr originFirstFile;    //指向初始修改时间最早的文件结点
    Treeptr originLatestFile;   //指向现在修改时间最晚的文件结点
}Dnode,*DirInfoptr;       //存放目录结点信息的结构体

typedef struct Qnode{
    char dirmaxname[520];
    int depth;                      //存放结点深度
    struct Qnode *next;             //指向队列中下一个结点
    Treeptr tnode;                   //指向目录树中对应的目录结点
} Qnode, *QueuePtr;  // 队列结点结构体
typedef struct {
    Qnode *front;
    Qnode *rear;
} LinkQueue;       // 队列结构体
// 定义队列结点长度
#define  LENG sizeof(Qnode)
//用于初始化队列
LinkQueue InitQueue();
// 用于生成遍历目录时所需的队列结点
int EnQueue(LinkQueue *Q, char* szFind,int nowdepth);
//用于删除遍历目录时生成的队列结点
int DelQueue(LinkQueue *Q, char* szFind,int* nowdepth);
//用于生成遍历目录树过程中需要的队列结点
int T_EnQueue(LinkQueue *Q,Treeptr tnode ,int nowdepth);
//用于删除遍历目录树过程中生成的队列结点
int T_DelQueue(LinkQueue *Q,Treeptr* tnode,int* nowdepth);
//给定带全路径名在目录树中找到结点
Treeptr findnode(char* maxname,Treeptr head);
//生成目录树目录结点
Treeptr createDIRnode(char* Dmaxname,char* Dname,FILETIME ftlastWriteTime,Treeptr parent);
//生成目录树文件结点
Treeptr createFILEnode(char* Fmaxname,WIN32_FIND_DATA* FindFileDate,Treeptr parent);
//遍历计算目录树深度
int getTreeDepth(Treeptr head);
//该函数用于遍历目录文件创建目录树并返回目录树树根指针
Treeptr createTREE(char *filePath,Ui::MainWindow* ui);
//该函数用于从指定文件中读取目录信息在目录树中统计统计文件信息。最早时间、最晚时间，文件总数，总的文件大小（不包含子目录）
int GetFileInfo(Treeptr head,Ui::MainWindow* ui);
//该函数用于从指定文件中读取文件操作并在目录树上进行模拟文件操作
int SimFileOper(Treeptr head,Ui::MainWindow* ui);
//该函数用于从指定文件中读取目录操作并在目录树上进行模拟目录操作
int SimDirOper(Treeptr head,Ui::MainWindow* ui);
//该函数用于读取指定文件中的目录信息并检查发生了哪些变化
int CheckInfoChange(Treeptr head,Ui::MainWindow* ui);
//该函数用于在目录树中查询指定文件或者目录信息
int FindInfo(Treeptr head,char* maxname,Ui::MainWindow* ui);
//该函数用于释放目录树所占的内存来防止内存泄漏
int ClearTree(Treeptr head);
//得到格式化修改时间
int getrighttime(FILETIME ftLastWriteTime,char* timestring);
//将文件信息写入sql文件
int writefile(char* filename,FILETIME ftlastWriteTime,long filesize,long filenum);
//将目录信息写入sql文件
int writedirectory(char* directoryname,FILETIME ftlastWriteTime,long dirnum);

#endif // TEST_ONE_H

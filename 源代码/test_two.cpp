#include<windows.h>
#include<stdio.h>
#include<time.h>
#include"test_one.h"
#include<string.h>
#include<mbstring.h>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

//该函数用于查询目录结点信息是否改变并输出查询结果
int CheckDiff(DirInfoptr thisnode,char* maxname,Ui::MainWindow* ui)
{
    //在该函数中查询的是目录信息结点，检查其是否发生变化以及发生了哪些变化；
    //目录结点信息包括大小，文件个数，最早时间文件以及最晚时间文件
    //其中首先要判断该目录在目录树中是否被删除，
    //再判断大小和文件个数是否改变，如果改变则输出改变
    //再判断单独判断最早时间文件和最晚时间文件是否发生变化
    //先判断其指针信息是否发生变化，
    //若没发生变化则再比较其文件信息是否发生了变化，but目前程无法实现该功能**********
    int point=0;      //存放该结点是否有信息改变，没有为0，有为1，最后作为函数返回值
    if(thisnode!=NULL)
    {
        if(thisnode->nownumber!=thisnode->originnumber)
        {
            point=1;
        }
        if(thisnode->nowsize!=thisnode->originsize)
        {
            point=1;
        }
        if(thisnode->nowFirstFile!=thisnode->originFirstFile)
        {
            point=1;
        }
        if(thisnode->nowLatestFile!=thisnode->originLatestFile)
        {
            point=1;
        }
        if(point==1)
        {
            ui->textBrowser->append(maxname);
            ui->textBrowser->append("该目录中文件总数变化为"+QString::number((thisnode->nownumber)-(thisnode->originnumber))+"总大小变化为"+QString::number((thisnode->nowsize)-(thisnode->originsize))+"字节\n");
            if(thisnode->nowFirstFile!=thisnode->originFirstFile)
            {
                if(thisnode->nowFirstFile!=NULL)
                {
                    ui->textBrowser->append("\n其中最早时间文件变为：");
                    ui->textBrowser->append(thisnode->nowFirstFile->maxname);
                    ui->textBrowser->append("大小为"+QString::number(thisnode->nowFirstFile->size)+"字节");
                    ui->textBrowser->append("时间为");
                    ui->textBrowser->append(thisnode->nowFirstFile->time);
                }
            }
            if(thisnode->nowLatestFile!=thisnode->originLatestFile)
            {
                if(thisnode->nowLatestFile!=NULL)
                {
                    ui->textBrowser->append("\n其中最晚时间文件变为：");
                    ui->textBrowser->append(thisnode->nowLatestFile->maxname);
                    ui->textBrowser->append("大小为"+QString::number(thisnode->nowLatestFile->size)+"字节");
                    ui->textBrowser->append("时间为");
                    ui->textBrowser->append(thisnode->nowLatestFile->time);
                }
            }
            if(thisnode->nownumber==0)
            {
                ui->textBrowser->append("该目录内文件数量为0，无最早和最晚时间文件\n");
            }
            ui->textBrowser->append("\n");
        }
    }
    else
    {
        point = 1;
        ui->textBrowser->append(maxname);
        ui->textBrowser->append("该目录已删除！\n");
    }
    return point;
}
//该函数用于遍历目录文件创建目录树并返回目录树树根指针
Treeptr createTREE(char *filePath,Ui::MainWindow* ui)
{
    char szFind[520];       //定义一个要找的文件路径的量
    WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息
    HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值
    char nowfilepath[520];      //存放当前访问文件路径
    char buffer[520];           //字符转换缓存区
    int nowdepth=1;             //存放当前目录层数
    LinkQueue dirqueue=InitQueue(); //得到初始目录队列
    long filenum=0;                 //存放文件数量
    long dirnum=0;                  //存放目录数量
    long Size=0;                    //存放目录总大小
    Treeptr headnode;               //指向目录树树根结点
    Treeptr parentnode;             //指向当前搜索的目录结点
    Treeptr nownode;                //指向当前生成目录树结点
    wchar_t *wfilename;             //存放转换字符的指针
    strcpy_s(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间
    strcat_s(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录
    wfilename = new wchar_t[strlen(szFind) + 1];
    mbstowcs(wfilename, szFind, strlen(szFind) + 1);
    hFind=FindFirstFile(wfilename,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件
    //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去
    if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
    {
        DWORD dwError = GetLastError();
        //printf("目录不存在！错误码为%lu\n",dwError);

        return 0;
    }
    //得到初始目录树结点
    headnode = (Treeptr)malloc(sizeof(Tnode));
    strcpy(headnode->maxname,filePath);
    strcpy(headnode->name,filePath);
    headnode->flag = 1;
    headnode->brother = NULL;
    headnode->kid = NULL;
    headnode->lastptr = NULL;
    headnode->Information = NULL;
    nownode = headnode;
    parentnode = headnode;
    while(1)
    {
        while(1)
        {                                                                //因为 FindFirstFile返回的 findData 中 dwFileAttributes项的值是一个组合的值
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //做位的与运算来判断所找到的项目是不是文件夹
            {
                if(FindFileData.cFileName[0]!='.')                        //每个目录下有个..目录可以返回上一层。如果不是返回目录
                //.表示当前目录，因为每个目录下面都有两个默认目录就是..和.分别表示上一级目录和当前目录
                {
                    dirnum++;
                    strcpy_s(szFind,filePath);
                    strcat_s(szFind,"\\");
                    wcstombs(buffer, FindFileData.cFileName, MAX_PATH);
                    strcat_s(szFind,buffer);
                    EnQueue(&dirqueue,szFind,nowdepth+1);
                    createDIRnode(szFind,buffer,FindFileData.ftLastWriteTime,parentnode);

                }
            }
            else
            {
                filenum++;
                Size+=(long)(FindFileData.nFileSizeLow/1024);            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小
                strcpy_s(nowfilepath,filePath);
                strcat_s(nowfilepath,"\\");
                wcstombs(buffer, FindFileData.cFileName, MAX_PATH);
                strcat_s(nowfilepath,buffer);
                createFILEnode(nowfilepath,&FindFileData,parentnode);

            }
            if(!FindNextFile(hFind,&FindFileData))                        //继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败
                break;                                                     //因此为0时，执行break
        }

        if(DelQueue(&dirqueue,szFind,&nowdepth))
        {
            int flag=0;
            strcpy(filePath,szFind);
            strcat(szFind,"\\*.*");
            wfilename = new wchar_t[strlen(szFind) + 1];
            mbstowcs(wfilename, szFind, strlen(szFind) + 1);
            hFind=FindFirstFile(wfilename,&FindFileData);
            while(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
            {
                ui->textBrowser->append(filePath);
                ui->textBrowser->append("该目录无法访问\n");
                if(DelQueue(&dirqueue,szFind,&nowdepth))
                {
                    strcpy(filePath,szFind);
                    strcat(szFind,"\\*.*");
                    wfilename = new wchar_t[strlen(szFind) + 1];
                    mbstowcs(wfilename, szFind, strlen(szFind) + 1);
                    hFind=FindFirstFile(wfilename,&FindFileData);
                }
                else
                {
                    flag=1;
                    break;
                }
            }
            if(flag)
            {

                break;
            }
            parentnode = findnode(filePath,headnode);
        }
        else
        {

            break;
        }
    }
    FindClose(hFind);//关闭句柄
    return headnode;
}
//该函数用于从指定文件中读取目录信息在目录树中统计统计文件信息。最早时间、最晚时间，文件总数，总的文件大小（不包含子目录）
int GetFileInfo(Treeptr head,Ui::MainWindow* ui)
{
    char maxname[300];
    Treeptr Pnode,Qnode;
    FILE* file = fopen("mystat.txt","r");
    if(file==NULL)
    {
        ui->textBrowser->append("mystat.txt文件无法读取!");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"stat dirs\n")!=0)
    {
        ui->textBrowser->append("读取文件错误！");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        Pnode = findnode(maxname,head);
        if(Pnode==NULL)
        {
            ui->textBrowser->append(maxname);
            ui->textBrowser->append("不存在！\n");
            continue;
        }
        if(Pnode->flag==1)
        {
            Pnode->Information = (DirInfoptr)malloc(sizeof(Dnode));
            Qnode = Pnode->kid;
            Pnode->Information->originFirstFile = NULL;
            Pnode->Information->originLatestFile = NULL;
            Pnode->Information->originnumber = 0;
            Pnode->Information->originsize = 0;
            if(Qnode==NULL)
            {
                ui->textBrowser->append(maxname);
                ui->textBrowser->append("该目录为空！\n");
                continue;
            }
            else
            {
                while(Qnode!=NULL)
                {
                    if(Qnode->flag==2)
                    {
                        (Pnode->Information->originnumber)++;
                        (Pnode->Information->originsize)+=(Qnode->size);
                        if((Pnode->Information->originFirstFile==NULL)||(Pnode->Information->originLatestFile==NULL))
                        {
                            Pnode->Information->originFirstFile = Pnode->Information->originLatestFile = Qnode;
                        }
                        else
                        {
                            int a = CompareFileTime(&(Pnode->Information->originFirstFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a>0)
                            {
                                Pnode->Information->originFirstFile = Qnode;
                            }
                            a = CompareFileTime(&(Pnode->Information->originLatestFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a<0)
                            {
                                Pnode->Information->originLatestFile = Qnode;
                            }
                        }
                    }
                    Qnode = Qnode->brother;
                }
                if(Pnode->Information->originnumber==0)
                {
                    ui->textBrowser->append(maxname);
                    ui->textBrowser->append("该目录内无文件！\n");
                    continue;
                }
                else
                {
                    ui->textBrowser->append(maxname);
                    ui->textBrowser->append("该目录中文件总数为"+QString::number(Pnode->Information->originnumber)+",总大小为"+QString::number(Pnode->Information->originsize)+"字节");
                    ui->textBrowser->append("\n其中最早时间文件为：");
                    ui->textBrowser->append(Pnode->Information->originFirstFile->maxname);
                    ui->textBrowser->append("大小为"+QString::number(Pnode->Information->originFirstFile->size)+"字节");
                    ui->textBrowser->append("时间为");
                    ui->textBrowser->append(Pnode->Information->originFirstFile->time);
                    ui->textBrowser->append("");
                    ui->textBrowser->append("其中最晚时间文件为：");
                    ui->textBrowser->append(Pnode->Information->originLatestFile->maxname);
                    ui->textBrowser->append("大小为"+QString::number(Pnode->Information->originLatestFile->size)+"字节");
                    ui->textBrowser->append("时间为");
                    ui->textBrowser->append(Pnode->Information->originLatestFile->time);
                    ui->textBrowser->append("\n");
                }
            }
        }
        else
        {
            //printf("%s该文件大小为%ld字节, 时间为%s\n\n\n",maxname,Pnode->size,Pnode->time);
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    return 1;
}
//该函数用于从指定文件中读取文件操作并在目录树上进行模拟文件操作
int SimFileOper(Treeptr head,Ui::MainWindow* ui)
{
    char maxname[300];
    char filename[300];
    char thisname[300];
    time_t unixtime;
    unsigned long long ll;
    Treeptr Pnode,Qnode;
    const char delimiter1[]=",";
    const char delimiter2[]="\n";
    char* str1;
    char* str2;
    char* endptr;
    FILE* file = fopen("myfile.txt","r");
    if(file==NULL)
    {
        ui->textBrowser->append("myfile.txt文件无法读取！\n");\
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"selected files\n")!=0)
    {
        ui->textBrowser->append("读取文件错误！\n");
        return 0;
    }
    while(fgets(maxname,299,file)!=NULL)
    {
        if(strcmp(maxname,"end of files\n")==0)
        {
            break;
        }
        strcpy(filename,maxname);
        strcpy(thisname,maxname);
        str1 = strtok(maxname,delimiter1);
        str1 = strtok(NULL,delimiter1);
        if(strcmp(str1,"M")==0)
        {
            Pnode = findnode(maxname,head);
            if(Pnode==NULL)
            {
                ui->textBrowser->append(maxname);
                ui->textBrowser->append("无法找到\n");
                continue;
            }
            str1 = strtok(filename,delimiter1);
            str1 = strtok(NULL,delimiter1);
            str1 = strtok(NULL,delimiter1);
            unixtime = strtoul(str1,&endptr,10);
            ll =  Int32x32To64(unixtime, 10000000) + 116444736000000000;
            Pnode->fltwritetime.dwLowDateTime = (DWORD)ll;
            Pnode->fltwritetime.dwHighDateTime = (ll>>32);
            endptr = NULL;
            getrighttime(Pnode->fltwritetime,Pnode->time);
            str1 = strtok(NULL,delimiter1);
            str2 = strtok(str1,delimiter2);
            Pnode->size = strtol(str2,&endptr,10);
            endptr = NULL;
        }
        else if(strcmp(str1,"D")==0)
        {
            Pnode = findnode(maxname,head);
            if(Pnode == NULL)
            {
                ui->textBrowser->append(maxname);
                ui->textBrowser->append("无法找到\n");
                continue;
            }
            if(Pnode->lastptr->kid == Pnode)
            {
                Pnode->lastptr->kid = Pnode->brother;
                if(Pnode->brother!=NULL)
                {
                    Pnode->brother->lastptr = Pnode->lastptr;
                }
            }
            else
            {
                Pnode->lastptr->brother = Pnode->brother;
                if(Pnode->brother!=NULL)
                {
                    Pnode->brother->lastptr = Pnode->lastptr;
                }
            }
            ClearTree(Pnode);
        }
        else if(strcmp(str1,"A")==0)
        {
            str1 = strtok(filename,delimiter1);
            str2 = strrchr(str1,'\\');
            *str2 = '\0';
            str2++;
            Pnode = findnode(str1,head);
            if(Pnode==NULL)
            {
                ui->textBrowser->append(maxname);
                ui->textBrowser->append("无法找到该文件的上级目录\n");
                continue;
            }
            if(Pnode->kid==NULL)
            {
                Pnode->kid = (Treeptr)malloc(sizeof(Tnode));
                Pnode->kid->brother=NULL;
                Pnode->kid->kid=NULL;
                Pnode->kid->flag=2;
                Pnode->kid->Information=NULL;
                Pnode->kid->lastptr=Pnode;
                strcpy(Pnode->kid->maxname,maxname);
                strcpy(Pnode->kid->name,str2);
                str1 = strtok(thisname,delimiter1);
                str1 = strtok(NULL,delimiter1);
                str1 = strtok(NULL,delimiter1);
                unixtime = strtoul(str1,&endptr,10);
                ll =  Int32x32To64(unixtime, 10000000) + 116444736000000000;
                Pnode->kid->fltwritetime.dwLowDateTime = (DWORD)ll;
                Pnode->kid->fltwritetime.dwHighDateTime = (ll>>32);
                getrighttime(Pnode->kid->fltwritetime,Pnode->kid->time);
                endptr = NULL;
                str1 = strtok(NULL,delimiter1);
                str2 = strtok(str1,delimiter2);
                Pnode->kid->size = strtol(str2,&endptr,10);
                endptr = NULL;
            }
            else
            {
                Pnode = Pnode->kid;
                while(Pnode->brother!=NULL)
                {
                    Pnode = Pnode->brother;
                }
                Pnode->brother = (Treeptr)malloc(sizeof(Tnode));
                Pnode->brother->brother=NULL;
                Pnode->brother->kid=NULL;
                Pnode->brother->flag=2;
                Pnode->brother->Information=NULL;
                Pnode->brother->lastptr=Pnode;
                strcpy(Pnode->brother->maxname,maxname);
                strcpy(Pnode->brother->name,str2);
                str1 = strtok(thisname,delimiter1);
                str1 = strtok(NULL,delimiter1);
                str1 = strtok(NULL,delimiter1);
                unixtime = strtoul(str1,&endptr,10);
                ll =  Int32x32To64(unixtime, 10000000) + 116444736000000000;
                Pnode->brother->fltwritetime.dwLowDateTime = (DWORD)ll;
                Pnode->brother->fltwritetime.dwHighDateTime = (ll>>32);
                getrighttime(Pnode->brother->fltwritetime,Pnode->brother->time);
                endptr = NULL;
                str1 = strtok(NULL,delimiter1);
                str2 = strtok(str1,delimiter2);
                Pnode->brother->size = strtol(str2,&endptr,10);
                endptr = NULL;
            }
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    ui->textBrowser->append("模拟文件操作执行成功！\n");
    return 1;
}
//该函数用于从指定文件中读取目录操作并在目录树上进行模拟目录操作
int SimDirOper(Treeptr head,Ui::MainWindow* ui)
{
    char maxname[300];
    Treeptr Pnode,Qnode;
    const char delimiter[]=",";
    char* str1;
    FILE* file = fopen("mydir.txt","r");
    if(file==NULL)
    {
        ui->textBrowser->append("mydir.txt文件无法读取！\n");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"selected dirs\n")!=0)
    {
        ui->textBrowser->append("读取文件错误！\n");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        str1 = strtok(maxname,delimiter);
        Pnode = findnode(str1,head);
        if(Pnode==NULL)
        {
            ui->textBrowser->append(str1);
            ui->textBrowser->append("该目录不存在！\n");
            continue;
        }
        if(Pnode->lastptr->kid == Pnode)
        {
            Pnode->lastptr->kid = Pnode->brother;
            if(Pnode->brother!=NULL)
            {
                Pnode->brother->lastptr = Pnode->lastptr;
            }
        }
        else
        {
            Pnode->lastptr->brother = Pnode->brother;
            if(Pnode->brother!=NULL)
            {
                Pnode->brother->lastptr = Pnode->lastptr;
            }
        }
        ClearTree(Pnode);
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    ui->textBrowser->append("模拟目录操作执行成功！\n");
    return 1;
}
//该函数用于读取指定文件中的目录信息并检查发生了哪些变化
int CheckInfoChange(Treeptr head,Ui::MainWindow* ui)
{
    char maxname[300];
    int number=0;
    Treeptr Pnode,Qnode;
    FILE* file = fopen("mystat.txt","r");
    if(file==NULL)
    {
        printf("mystat.txt文件无法读取！\n");
        return 0;
    }
    fgets(maxname,100,file);
    if(strcmp(maxname,"stat dirs\n")!=0)
    {
        printf("读取文件错误！\n");
        return 0;
    }
    while(fscanf(file,"%s",maxname)==1)
    {
        if(strcmp(maxname,"end")==0)
        {
            break;
        }
        Pnode = findnode(maxname,head);
        if(Pnode==NULL)
        {
            number += CheckDiff(NULL,maxname,ui);
            continue;
        }
        if(Pnode->flag==1)
        {
            Qnode = Pnode->kid;
            Pnode->Information->nowFirstFile = NULL;
            Pnode->Information->nowLatestFile = NULL;
            Pnode->Information->nownumber = 0;
            Pnode->Information->nowsize = 0;
            if(Qnode==NULL)
            {
                number += CheckDiff(Pnode->Information,maxname,ui);
                continue;
            }
            else
            {
                while(Qnode!=NULL)
                {
                    if(Qnode->flag==2)
                    {
                        (Pnode->Information->nownumber)++;
                        (Pnode->Information->nowsize)+=(Qnode->size);
                        if((Pnode->Information->nowFirstFile==NULL)||(Pnode->Information->nowLatestFile==NULL))
                        {
                            Pnode->Information->nowFirstFile = Pnode->Information->nowLatestFile = Qnode;
                        }
                        else
                        {
                            int a = CompareFileTime(&(Pnode->Information->nowFirstFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a>0)
                            {
                                Pnode->Information->nowFirstFile = Qnode;
                            }
                            a = CompareFileTime(&(Pnode->Information->nowLatestFile->fltwritetime),&(Qnode->fltwritetime));
                            if(a<0)
                            {
                                Pnode->Information->nowLatestFile = Qnode;
                            }
                        }
                    }
                    Qnode = Qnode->brother;
                }
                number += CheckDiff(Pnode->Information,maxname,ui);
            }
        }
    }
    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
    ui->textBrowser->append("\n  共有"+QString::number(number)+"条目录信息发生变化");
    printf("\n   共有%d条目录信息发生变化\n\n",number);
    return 1;
}
//该函数用于在目录树中查询指定文件或者目录信息
int FindInfo(Treeptr head,char*maxname,Ui::MainWindow* ui)
{
    Treeptr Pnode,Qnode;
    Pnode = findnode(maxname,head);
    if(Pnode==NULL)
    {
        ui->textBrowser->append("该目录或文件不存在！\n");
        return 0;
    }
    if(Pnode->flag==1)
    {
        Pnode->Information = (DirInfoptr)malloc(sizeof(Dnode));
        Qnode = Pnode->kid;
        Pnode->Information->nowFirstFile = NULL;
        Pnode->Information->nowLatestFile = NULL;
        Pnode->Information->nownumber = 0;
        Pnode->Information->nowsize = 0;
        if(Qnode==NULL)
        {
            ui->textBrowser->append("该目录为空！\n");
            return 1;
        }
        else
        {
            while(Qnode!=NULL)
            {
                if(Qnode->flag==2)
                {
                    (Pnode->Information->nownumber)++;
                    (Pnode->Information->nowsize)+=(Qnode->size);
                    if((Pnode->Information->nowFirstFile==NULL)||(Pnode->Information->nowLatestFile==NULL))
                    {
                        Pnode->Information->nowFirstFile = Pnode->Information->nowLatestFile = Qnode;
                    }
                    else
                    {
                        int a = CompareFileTime(&(Pnode->Information->nowFirstFile->fltwritetime),&(Qnode->fltwritetime));
                        if(a>0)
                        {
                            Pnode->Information->nowFirstFile = Qnode;
                        }
                        a = CompareFileTime(&(Pnode->Information->nowLatestFile->fltwritetime),&(Qnode->fltwritetime));
                        if(a<0)
                        {
                            Pnode->Information->nowLatestFile = Qnode;
                        }
                    }
                }
                Qnode = Qnode->brother;
            }
            if(Pnode->Information->nownumber==0)
            {
                ui->textBrowser->append("该目录内无文件！\n");
                return 1;
            }
            else
            {
                ui->textBrowser->append(maxname);
                ui->textBrowser->append("该目录中文件总数为"+QString::number(Pnode->Information->nownumber)+",总大小为"+QString::number(Pnode->Information->nowsize)+"字节");
                ui->textBrowser->append("\n其中最早时间文件为：");
                ui->textBrowser->append(Pnode->Information->nowFirstFile->maxname);
                ui->textBrowser->append("大小为"+QString::number(Pnode->Information->nowFirstFile->size)+"字节");
                ui->textBrowser->append("时间为");
                ui->textBrowser->append(Pnode->Information->nowFirstFile->time);
                ui->textBrowser->append("");
                ui->textBrowser->append("其中最晚时间文件为：");
                ui->textBrowser->append(Pnode->Information->nowLatestFile->maxname);
                ui->textBrowser->append("大小为"+QString::number(Pnode->Information->nowLatestFile->size)+"字节");
                ui->textBrowser->append("时间为");
                ui->textBrowser->append(Pnode->Information->nowLatestFile->time);
                ui->textBrowser->append("\n");
            }
        }
    }
    else
    {
        ui->textBrowser->append(maxname);
        ui->textBrowser->append("该文件大小为"+QString::number(Pnode->size)+"字节，时间为"+Pnode->time);
        ui->textBrowser->append("\n");
    }
    return 1;
}
//该函数用于释放目录树所占的内存来防止内存泄漏
int ClearTree(Treeptr head)
{
    LinkQueue TREEqueue= InitQueue();
    Treeptr Pnode;
    Treeptr Tnode;
    int nowdepth=1;
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
                Tnode = Pnode;
                Pnode = Pnode->brother;
                free(Tnode);
            }
        }
        if(T_DelQueue(&TREEqueue,&Pnode,&nowdepth)==0)
        {
            break;
        }
        else
        {
            Tnode = Pnode;
            Pnode = Pnode->kid;
            if(Tnode->Information!=NULL)
            {
                free(Tnode->Information);
            }
            free(Tnode);
        }
    }
    if(head->Information!=NULL)
    {
        free(head->Information);
    }
    free(head);
    return 1;
}

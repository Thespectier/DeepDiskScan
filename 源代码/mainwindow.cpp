#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "test_one.h"
#include<stdio.h>
#include<string.h>
#include<mbstring.h>
#include<windows.h>
#include<qfiledialog.h>
#include<QtWidgets>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->document()->setMaximumBlockCount(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


Treeptr DirTree = NULL;

void MainWindow::setcursor()
{
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);
}


void MainWindow::on_scan_directory_triggered()
{
    QWidget window;
    window.setWindowTitle("Select Directory");
    PVOID oldvalue = NULL;
    char buffer[MAX_PATH];
    QFileDialog dialog(&window);
    dialog.setFileMode(QFileDialog::Directory);
    Wow64DisableWow64FsRedirection(&oldvalue);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 获取所选目录的路径
            QString directory = files.first();
            directory.replace("/","\\");
            char filePath[520];
            std::string stdstr = directory.toStdString();
            strcpy_s(filePath,stdstr.c_str());
            //.....................................
            char szFind[520];       //定义一个要找的文件路径的量
            WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息
            HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值
            int maxpath=0;                 //存放最长路径长度
            int nowpath=0;                //存放当前路径长度
            char maxfilepath[520];      //存放最长路径文件名称
            int maxdepth=1;             //存放最深目录层数
            int nowdepth=1;             //存放当前目录层数
            LinkQueue dirqueue=InitQueue(); //得到初始目录队列
            long number=0;                  //存放目录和文件总数
            long filenum=0;                 //存放文件数量
            long dirnum=0;                  //存放目录数量
            long Size=0;

            strcpy_s(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间
            strcat_s(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录
            wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
            mbstowcs(wfilename, szFind, strlen(szFind) + 1);
            ui->textBrowser->append("开始扫描目录："+directory);
            hFind=FindFirstFile(wfilename,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件

            //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去

            if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
            {
                DWORD dwLastError = GetLastError();
                // 根据错误代码判断原因
                switch (dwLastError) {
                case ERROR_FILE_NOT_FOUND:
                    ui->textBrowser->append("文件或目录不存在\n");
                    break;
                case ERROR_PATH_NOT_FOUND:
                    ui->textBrowser->append("路径不存在\n");
                    break;
                case ERROR_ACCESS_DENIED:
                    ui->textBrowser->append("拒绝访问\n");
                    break;
                case ERROR_INVALID_PARAMETER:
                    ui->textBrowser->append("参数无效\n");
                    break;
                default:
                    ui->textBrowser->append("未知错误\n");
                }
                ui->textBrowser->append(szFind);
                ui->textBrowser->append("该目录不存在！\n");
                setcursor();
                return;
            }

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
                            number++;
                            strcpy_s(szFind,filePath);
                            strcat_s(szFind,"\\");
                            wcstombs(buffer, FindFileData.cFileName, MAX_PATH);
                            strcat_s(szFind,buffer);
                            EnQueue(&dirqueue,szFind,nowdepth+1);
                        }
                    }
                    else
                    {
                        filenum++;
                        number++;
                        Size += (long)(FindFileData.nFileSizeLow/1024);            //对文件的大小进行统计  FindFileData.nFileSizeLow是字节大小
                        wcstombs(buffer, FindFileData.cFileName, MAX_PATH);
                        //printf("%ld  %s\\%s\n%d字节\n",filenum,filePath,FindFileData.cFileName,FindFileData.nFileSizeLow);//输出目录下的文件的路径和名称和大小
                        nowpath=strlen(filePath)+strlen(buffer)+1;
                        if(nowpath>maxpath)
                        {
                            maxpath=nowpath;
                            strcpy_s(maxfilepath,filePath);
                            strcat_s(maxfilepath,"\\");
                            strcat_s(maxfilepath,buffer);
                        }
                    }
                    if(!FindNextFile(hFind,&FindFileData))                        //继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败
                    {
                        break;
                    }                                                  //因此为0时，执行break
                    else
                    {
                        //printf("访问%s\n",FindFileData.cFileName);
                    }
                }

                if(DelQueue(&dirqueue,szFind,&nowdepth))
                {
                    int flag=0;
                    strcpy(filePath,szFind);
                    strcat(szFind,"\\*.*");
                    wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
                    mbstowcs(wfilename, szFind, strlen(szFind) + 1);
                    hFind=FindFirstFile(wfilename,&FindFileData);
                    if(nowdepth>maxdepth)
                    {
                        maxdepth=nowdepth;
                    }
                    while(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
                    {
                        //printf("%s该目录无法访问\n",filePath);
                        if(DelQueue(&dirqueue,szFind,&nowdepth))
                        {
                            strcpy(filePath,szFind);
                            strcat(szFind,"\\*");
                            wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
                            mbstowcs(wfilename, szFind, strlen(szFind) + 1);
                            hFind=FindFirstFile(wfilename,&FindFileData);
                            if(nowdepth>maxdepth)
                            {
                                maxdepth=nowdepth;
                            }
                        }
                        else
                        {
                            flag=1;
                            break;
                        }
                    }
                    if(flag)
                    {
                        ui->textBrowser->append("扫描完毕!");
                        setcursor();
                        break;
                    }
                }
                else
                {
                    ui->textBrowser->append(directory+"扫描完毕!");
                    setcursor();
                    break;
                }
            }
            FindClose(hFind);//关闭句柄
            Wow64RevertWow64FsRedirection(oldvalue);
            //printf("最长带全路径文件为%s\n长度为%d\n目录层数为%d\n",maxfilepath,maxpath,maxdepth);
            ui->textBrowser->append("最长带全路径文件为");
            ui->textBrowser->append(maxfilepath);
            ui->textBrowser->append("长度为"+QString::number(maxpath));
            ui->textBrowser->append("目录总大小为："+QString::number(Size)+"kb");
            ui->textBrowser->append("目录层数为"+QString::number(maxdepth));;
            //printf("文件夹和文件数量总和为：%ld\n文件夹数量为%ld\n文件数量为%ld\n",number,dirnum,filenum);
            ui->textBrowser->append("文件夹和文件数量总和为："+QString::number(number));
            ui->textBrowser->append("文件夹数量为："+QString::number(dirnum));
            ui->textBrowser->append("文件数量为："+QString::number(filenum));
            ui->textBrowser->append("\n\n.............................................\n\n");
            //......................................
            setcursor();
        }
    }

}


void MainWindow::on_get_SQLfiles_triggered()
{
    QWidget window;
    window.setWindowTitle("Select Directory");
    PVOID oldvalue = NULL;
    char buffer[MAX_PATH];
    QFileDialog dialog(&window);
    dialog.setFileMode(QFileDialog::Directory);
    Wow64DisableWow64FsRedirection(&oldvalue);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 获取所选目录的路径
            QString directory = files.first();
            directory.replace("/","\\");
            char filePath[520];
            std::string stdstr = directory.toStdString();
            strcpy_s(filePath,stdstr.c_str());
            //.....................................
            char szFind[520];       //定义一个要找的文件路径的量
            WIN32_FIND_DATA FindFileData;//WIN32_FIND_DATA结构描述了一个由FindFirstFile, FindFirstFileEx, 或FindNextFile函数查找到的文件信息
            HANDLE hFind;                //定义一个句柄 用于FindFirstFile()返回的值
            char nowfilepath[520];      //存放当前路径文件名称
            int nowdepth=1;             //存放当前目录层数
            LinkQueue dirqueue=InitQueue(); //得到初始目录队列
            long filenum=0;                 //存放文件数量
            long dirnum=0;                  //存放目录数量
            int filesqlnum=0;               //存放生成filesql文件数量
            int dirsqlnum=0;                //存放生成dirsql文件数量

            strcpy_s(szFind,filePath);     //把从后者地址开始且含有NULL结束符的字符串赋值到前者开始的地址空间
            strcat_s(szFind,"\\*.*");      //把后者所指字符串添加到前者结尾处(覆盖前者结尾处的'\0')并添加'\0'。利用通配符找这个目录下的所有文件，包括目录
            wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
            mbstowcs(wfilename, szFind, strlen(szFind) + 1);
            ui->textBrowser->append("开始生成SQL文件...");
            hFind=FindFirstFile(wfilename,&FindFileData);  //FindFirstFile函数返回HANDLE类型的值 其中FILE_ATTRIBUTE_DIRECTORY 表明文件是一个目录文件

            //通过FindFirstFile()函数,根据当前的文件存放路径查找该文件来把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去

            if(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
            {
                DWORD dwLastError = GetLastError();
                // 根据错误代码判断原因
                switch (dwLastError) {
                case ERROR_FILE_NOT_FOUND:
                    ui->textBrowser->append("文件或目录不存在\n");
                    break;
                case ERROR_PATH_NOT_FOUND:
                    ui->textBrowser->append("路径不存在\n");
                    break;
                case ERROR_ACCESS_DENIED:
                    ui->textBrowser->append("拒绝访问\n");
                    break;
                case ERROR_INVALID_PARAMETER:
                    ui->textBrowser->append("参数无效\n");
                    break;
                default:
                    ui->textBrowser->append("未知错误\n");
                }
                ui->textBrowser->append(szFind);
                ui->textBrowser->append("该目录不存在！\n");
                setcursor();
                return;
            }

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
                            dirsqlnum = writedirectory(szFind,FindFileData.ftLastWriteTime,dirnum);
                        }
                    }
                    else
                    {
                        filenum++;
                        wcstombs(buffer, FindFileData.cFileName, MAX_PATH);
                        //printf("%ld  %s\\%s\n%d字节\n",filenum,filePath,FindFileData.cFileName,FindFileData.nFileSizeLow);//输出目录下的文件的路径和名称和大小
                        strcpy(nowfilepath,filePath);
                        strcat(nowfilepath,"\\");
                        strcat(nowfilepath,buffer);
                        filesqlnum = writefile(nowfilepath,FindFileData.ftLastWriteTime,(long)(FindFileData.nFileSizeLow/1024),filenum);
                    }
                    if(!FindNextFile(hFind,&FindFileData))                        //继续查找FindFirstFile函数搜索后的文件    非零表示成功，零表示失败
                    {
                        break;
                    }                                                  //因此为0时，执行break
                    else
                    {
                        //printf("访问%s\n",FindFileData.cFileName);
                    }
                }

                if(DelQueue(&dirqueue,szFind,&nowdepth))
                {
                    int flag=0;
                    strcpy(filePath,szFind);
                    strcat(szFind,"\\*.*");
                    wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
                    mbstowcs(wfilename, szFind, strlen(szFind) + 1);
                    hFind=FindFirstFile(wfilename,&FindFileData);
                    while(INVALID_HANDLE_VALUE==hFind)             //如果返回的是INVALID_HANDLE_VALUE表明目录不存在
                    {
                        //printf("%s该目录无法访问\n",filePath);
                        if(DelQueue(&dirqueue,szFind,&nowdepth))
                        {
                            strcpy(filePath,szFind);
                            strcat(szFind,"\\*");
                            wchar_t *wfilename = new wchar_t[strlen(szFind) + 1];
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
                        ui->textBrowser->append("扫描完毕!");
                        setcursor();
                        break;
                    }
                }
                else
                {
                    ui->textBrowser->append(directory+"扫描完毕!");
                    setcursor();
                    break;
                }
            }
            FindClose(hFind);//关闭句柄
            Wow64RevertWow64FsRedirection(oldvalue);
            //printf("文件夹和文件数量总和为：%ld\n文件夹数量为%ld\n文件数量为%ld\n",number,dirnum,filenum);
            ui->textBrowser->append("SQL文件生成成功！");
            ui->textBrowser->append("其中fileSQL文件个数为："+QString::number(filesqlnum));
            ui->textBrowser->append("其中dirSQL文件个数为："+QString::number(dirsqlnum));
            ui->textBrowser->append("\n\n.............................................\n\n");
            //......................................
            setcursor();
        }
    }
}



void MainWindow::on_generate_DirTree_triggered()
{
    QWidget window;
    window.setWindowTitle("Select Directory");
    PVOID oldvalue = NULL;
    char buffer[MAX_PATH];
    QFileDialog dialog(&window);
    dialog.setFileMode(QFileDialog::Directory);
    Wow64DisableWow64FsRedirection(&oldvalue);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            // 获取所选目录的路径
            QString directory = files.first();
            directory.replace("/","\\");
            QString lowername = directory.toLower();
            char filePath[520];
            std::string stdstr = lowername.toStdString();
            strcpy_s(filePath,stdstr.c_str());
            //.....................................
            ui->textBrowser->append("开始生成目录树...");
            //strcpy(filePath,"c:\\windows");
            DirTree = createTREE(filePath,ui);
            ui->textBrowser->append("目录树生成成功！");
            int treedepth=getTreeDepth(DirTree);
            ui->textBrowser->append("目录树层数为："+QString::number(treedepth));
            //......................................
            Wow64RevertWow64FsRedirection(oldvalue);
            ui->textBrowser->append("\n\n.............................................\n\n");
            setcursor();
        }
    }
}


void MainWindow::on_clear_DirTree_triggered()
{
    ClearTree(DirTree);
    ui->textBrowser->append("目录树空间释放成功！");
    ui->textBrowser->append("\n\n.............................................\n\n");
    setcursor();
}


void MainWindow::on_get_FileInfo_2_triggered()
{
    GetFileInfo(DirTree,ui);
    ui->textBrowser->append("目录信息获取成功！");
    ui->textBrowser->append("\n\n.............................................\n\n");
    setcursor();
}


void MainWindow::on_check_InfoChange_triggered()
{
    CheckInfoChange(DirTree,ui);
    ui->textBrowser->append("\n\n.............................................\n\n");
    setcursor();
}


void MainWindow::on_sim_DirOper_triggered()
{
    SimDirOper(DirTree,ui);
    ui->textBrowser->append("\n\n.............................................\n\n");
    setcursor();
}

void MainWindow::on_sim_FileOper_triggered()
{
    SimFileOper(DirTree,ui);
    ui->textBrowser->append("\n\n.............................................\n\n");
    setcursor();
}


void MainWindow::on_get_SpecInfo_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(nullptr, "Get_SpeciInfo", "请输入待查询文件或目录路径", QLineEdit::Normal, "", &ok);
    if (ok)
    {
        char filePath[520];
        std::string stdstr = text.toStdString();
        strcpy_s(filePath,stdstr.c_str());
        FindInfo(DirTree,filePath,ui);
        ui->textBrowser->append("\n\n.............................................\n\n");
        setcursor();
    }
}


void MainWindow::on_use_document_triggered()
{
    // 创建新的 QDialog 窗口
    QDialog dialog;
    // 将窗口大小调整为 960 x 640
    dialog.resize(960, 640);
    // 在 QDialog 窗口中添加自定义文本
    QTextBrowser help(&dialog);
    help.append("\n.............................这是一个使用文档..........................\n\n");
    help.append("scan_directory：");
    help.append("点击后选择想要扫描的目录或文件，等待扫描结果\n\n");
    help.append("get_SQLfiles:");
    help.append("点击后选择目录或文件，等待扫描相关信息并生成SQL文件\n\n");
    help.append("DirTree_Operations: ");
    help.append("点击后选择generate_DirTree来选择目录构建目录树");
    help.append("点击后选择clear_DirTree来销毁当前目录树\n\n");
    help.append("DirTree_Information: ");
    help.append("点击后选择get_Fileinfo来获取目录树中mystat.txt文件中指定的目录信息");
    help.append("点击后选择check_InfoChange再次查询目录树中mystat.txt文件中指定目录信息，并显示出信息改变");
    help.append("点击后选择get_SpecInfo来输入待查询目录或文件路径，在目录树中进行查询并显示查询结果\n\n");
    help.append("Simulate_Operations:");
    help.append("点击后选择Sim_FileOper从myfile.txt文件中获取指定文件信息并在目录树中进行操作");
    help.append("点击后选择Sim_DirOper从mydir.txt文件中获取指定目录信息并在目录树中进行操作");

    // 设置文本框大小
    help.resize(960,640);
    // 设置滚动条
    help.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    // 设置文本样式
    help.setStyleSheet("font-size: 18px; color: black;");
    // 显示 QDialog 窗口
    dialog.exec();
}


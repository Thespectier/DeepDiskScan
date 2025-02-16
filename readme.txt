本程序用于快速深度扫描Windows系统中的磁盘文件，采用c语言实现。
本程序在Windows操作系统下运行（推荐Win11或Win10）。
..........................这是一个使用文档..........................

scan_directory：
点击后选择想要扫描的目录或文件，等待扫描结果

get_SQLfiles:
点击后选择目录或文件，等待扫描相关信息并生成SQL文件

DirTree_Operations: 
点击后选择generate_DirTree来选择目录构建目录树
点击后选择clear_DirTree来销毁当前目录树

DirTree_Information: 
点击后选择get_Fileinfo来获取目录树中mystat.txt文件中指定的目录信息
点击后选择check_InfoChange再次查询目录树中mystat.txt文件中指定目录信息，并显示出信息改变
点击后选择get_SpecInfo来输入待查询目录或文件路径，在目录树中进行查询并显示查询结果

Simulate_Operations:
点击后选择Sim_FileOper从myfile.txt文件中获取指定文件信息并在目录树中进行操作
点击后选择Sim_DirOper从mydir.txt文件中获取指定目录信息并在目录树中进行操作

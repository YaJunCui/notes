# Boost安装

## 1 以asio为例

从 www.boost.org 中下载boost函数库，里面有个HTML形式的帮助文档，打开index.htm或者index.html即可看到分层别类的帮助信息。阅读帮助文档是学习新知识最直接最方便的方法。

date_time需要编译才能使用其完整功能（不编译其实也行，只是每次编译器都会对包含的头文件进行编译，速度相对于已经编译好的文件而言会慢一些），编译方法如下：

1. 运行bootstrap.bat，生成bjam.exe和b2.exe这两个文件本质是一样的，只是bjam.exe比较陈旧一些；
2. 选择部分编译(在asio的Building Boost Libraries查找)：打开命令行，进入到b2.exe所在的目录，然后在命令行下输入b2 --with-date_time
3. 编译出的库文件在stage文件夹下

# Java调用R——Rserve

## 1 Rserve介绍

Rserve是一个基于TCP/IP协议的，允许R语言与其他语言进行通信的程序，支持C/C++、Java、Python等语言。Rserve提供远程连接、认证以及文件传输等功能。可以将R作为后台的一个“服务器”，来处理统计建模、数据分析、绘图等任务。

## 2 Rserve环境配置

该Rserve环境设置默认Java、Tomcat和R的环境已经正确配置。如果没有配置Java和R的环境，请先正确配置Java和R的运行环境。

### 2.1 环境

* Windows 7 64bit
* Eclipse Mars.1 Release (4.5.1)
* Apache-Tomcat-8.0.9
* R version 3.2.2
* Rserve 1.7-3

### 2.2 环境配置的步骤

* 2.2.1 在R中安装Rserve扩展包。R中安装命令：install.packages("Rserve")

![在R中安装Rserve扩展包]()

运行之后，rJava默认被安装到R的安装路径中library文件夹中。例如：D:\Program Files\R\R-3.2.2\library\Rserve。
2.2.2 新建一个Java web工程，将D:\Program Files\R\R-3.2.2\library\Rserve\java下的REngine.java和Rserve.java导入到/WebContent/WEB-INF/lib目录下，然后就可以进行R语言的调用。
3.从网页中输入R的标准语句
3.1 首先启动Rserve。在RGui中分别执行library("Rserve")和Rserve()，即可启动Rserve。
 
3.2 在Java Web工程中添加R语句输入和执行页面。
R语句输入页面index.jsp，核心代码：
 
输入R标准语句，提交后转发到R的执行页面doAction.jsp，核心代码：
 
首先添加在R语句执行界面导入REngine和Rserve包，接着获取输入页面输入的R指令，然后通过RConnection()函数连接Rserve服务程序。本示例可以执行任何R语句输出结果是数组的指令，将evel()函数执行的结果转为Java数组进行输出。
3.2.3 将工程部署到Tomcat服务器上即可在输入页面进行输入。
 
 
4 存在的问题
4.1 Rserve在Windows上运行存在一定的局限性，而且Rserve不支持多线程。
4.2 虽然通过以上的方法可以执行普通的R语句，但是不能执行所有R中允许的形式。
例如：在RGui中执行c(1:10,"next",2)，结果如下：
 
但是使用JRI不能进行执行，因为REXP的asDoubles()方法不支持字符串。
 
 
 
 
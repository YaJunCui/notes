# man : 查看 Linux 在线帮助

Linux 的 man 手册共有以下几个章节：

|章节|代表內容|
|:---:|:---|
|1|普通的 Linux 命令|
|2|系统调用，如open、write之类的|
|3|库函数，如printf、fread，大部分是C的函数库(libc)|
|4|特殊文件，也就是/dev下的各种设备文件|
|5|文件的格式，比如passwd，就会说明这个文件中各个字段的含义|
|6|给游戏留的，由各个游戏自己定义|
|7|惯例与协定等，例如Linux档案系统、网络协定、ASCII code等等的说明|
|8|系统管理用的命令，这些命令只能由root使用，如ifconfig|
|9|跟kernel有关的文件|

对于像open，kill这种既有命令，又有系统调用的来说，man open则显示的是open(1)，也就是从最前面的section开始，如果想查看open系统调用的话，就得 man 2 open
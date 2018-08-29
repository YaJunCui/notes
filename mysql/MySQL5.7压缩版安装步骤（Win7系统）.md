# MySQL5.7压缩版安装步骤（Win7系统）

1. 从MySQL官网下载mysql-5.7-xxx.zip文件。将文件解压到某硬盘，本例解压到`E:\mysql-5.7`。
2. 配置系统环境变量，`Path：E:\mysql-5.7\bin`，以便mysql命令全局可用（重启后生效）。
3. 在mysql服务器根目录('E:\mysql-5.7')创建my.ini文件。文件内容如下：
    ```mysql
    [mysql]
    # 设置mysql客户端默认字符集
    default-character-set=utf8 
    [mysqld]
    # 设置3306端口
    port=3306 
    # 设置mysql的安装目录
    basedir=E:\mysql-5.7
    # 设置mysql数据库的数据的存放目录
    datadir=E:\mysql-5.7\data
    # 允许最大连接数
    max_connections=200
    # 服务端使用的字符集默认为8比特编码的latin1字符集
    character-set-server=utf8
    # 创建新表时将使用的默认存储引擎
    default-storage-engine=INNODB
    ```
    > 注：my.ini文件的编码必须为utf-8，否则报错。

4. 以管理员身份运行cmd，进入：`E:\mysql-5.7\bin`。然后执行`mysqld  --initialize-insecure`，执行完后，可以看到安装目录下自动生成了data目录，并包含很多文件。

5. 启动mysql服务器：`net start mysql`。

6. 检查是否能登录上去：`mysql -uroot -p`，因为没设置密码，所以第一次登录不用输入密码，直接回车就能登录成功。

7. 设置密码：
    `ALTER USER 'root'@'localhost' IDENTIFIED BY '新密码' PASSWORD EXPIRE NEVER;`
    -- 密码永不过期,将NEVER去掉则是有过期时间。

8. 停止MySQL服务器：`net stop mysql`。
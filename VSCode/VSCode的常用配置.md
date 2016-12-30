# VSCode的常用配置

1 如何从 Ubuntu 终端在任何地方启动 Code？

需要将需要创建 `/home/cui/.local/share/umake/web/visual-studio-code/bin/code` （这是使用umake命令安装的默认安装目录）的一个链接 `/usr/local/bin/code`。

```shell
ln -s /home/cui/.local/share/umake/web/visual-studio-code/bin/code /usr/local/bin/code
```

然后就可以在终端中运行以下命令启动 Visual Studio Code 了。

```shell
code .
```

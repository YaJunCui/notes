# muduo编译和安装.md

源文件tar包的下载地址：<http://code.google.com/p/muduo/downloads/list>，此处以muduo-0.8.2-beta.tar.gz为例。

muduo使用了Linux较新的系统调用（主要是timerfd和eventfd），要求Linux的内核版本大于2.6.28。我自己用Debian 6.0 Squeeze / Ubuntu 10.04 LTS作为主要开发环境（内核版本2.6.32），以g++ 4.4为主要编译器版本，在32-bit和64-bit x86系统都编译测试通过。muduo在Fedora 13和CentOS 6上也能正常编译运行，还有热心网友为Arch Linux编写了AUR文件。
如果要在较旧的Linux 2.6内核上使用muduo，可以参考backport.diff来修改代码。不过这些系统上没有充分测试，仅仅是编译和冒烟测试通过。另外muduo也可以运行在嵌入式系统中，我在Samsung S3C2440开发板（ARM9）和Raspberry Pi（ARM11）上成功运行了muduo的多个示例。代码只需略作改动，请参考armlinux.diff。
muduo采用CMake为build system，安装方法如下：

```shell
sudo apt-get install cmake
```

muduo依赖于Boost，也很容易安装：

```shell
sudo apt-get install libboost-dev libboost-test-dev
```

muduo有三个非必需的依赖库：curl、c-ares DNS、Google Protobuf，如果安装了这三个库，cmake会自动多编译一些示例。安装方法如下：

```shell
sudo apt-get install libcurl4-openssl-dev libc-ares-dev
sudo apt-get install protobuf-compiler libprotobuf-dev
```

muduo的编译方法很简单：

```shell
tar zxf muduo-0.8.2-beta.tar.gz
cd muduo/
./build.sh -j2
```

编译muduo 库和它自带的例子，生成的可执行文件和静态库文件
分别位于../build/debug/{bin,lib}

```shell
./build.sh install
```

以上命令将muduo 头文件和库文件安装到../build/debug-install/{include,lib}，以便muduo-protorpc 和muduo-udns 等库使用
如果要编译release版（以-O2优化），可执行：

```shell
BUILD_TYPE=release ./build.sh -j2
```

编译muduo 库和它自带的例子，生成的可执行文件和静态库文件
分别位于../build/release/{bin,lib}

```shell
BUILD_TYPE=release ./build.sh install
```

以上命令将muduo 头文件和库文件安装到../build/release-install/{include,lib}，以便muduo-protorpc 和muduo-udns 等库使用

在muduo 1.0正式发布之后，BUILD_TYPE的默认值会改成release。
编译完成之后请试运行其中的例子，比如bin/inspector_test，然后通过浏览器访问 <http://10.0.0.10:12345/> 或 <http://10.0.0.10:12345/proc/status>，其中10.0.0.10替换为你的Linuxbox的IP。
在自己的程序中使用muduo
muduo是静态链接的C++程序库，使用muduo库的时候，只需要设置好头文件路径（例如../build/debug-install/include）和库文件路径（例如../build/debug-install/lib）并链接相应的静态库文件（-lmuduo_net -lmuduo_base）即可。下面这个示范项目展示了如何使用CMake和普通makefile编译基于muduo的程序：<https://github.com/chenshuo/muduo-tutorial>


注意：如果要在编译程序的时候不带include和lib的路径，直接执行拷贝`cp -r muduo /usr/include/`将`build/debug-install/include/muduo`拷贝到`/usr/include`下。
  和 `cp * /usr/lib`把`build/debug-install/lib`下的库拷贝到`/usr/lib`下
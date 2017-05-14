# Boost.Asio 基本原理

这一章涵盖了使用 Boost.Asio 时必须知道的一些事情。我们也将深入研究比同步编程更复杂、更有乐趣的异步编程。

## 网络API

这一部分包含了当使用 Boost.Asio 编写网络应用程序时必须知道的事情。

## Boost.Asio命名空间

Boost.Asio 的所有内容都包含在 boost::asio 命名空间或者其子命名空间内。

* boost::asio：这是核心类和函数所在的地方。重要的类有 io\_service 和 streambuf。类似 read, read\_at, read_until 方法，它们的异步方法，它们的写方法和异步写方法等自由函数也在这里。
* boost::asio::ip：这是网络通信部分所在的地方。重要的类有 address，endpoint，tcp，udp 和 icmp，重要的自由函数有 connect 和 async_connect。要注意的是在 boost::asio::ip::tcp::socket中间，socket 只是 boost::asio::ip::tcp 类中间的一个 typedef 关键字。
* boost::asio::error：这个命名空间包含了调用 I/O 例程时返回的错误码
* boost::asio::ssl：包含了 SSL 处理类的命名空间
* boost::asio::local：这个命名空间包含了 POSIX 特性的类
* boost::asio::windows：这个命名空间包含了 Windows 特性的类

## IP地址

对于IP地址的处理，Boost.Asio提供了ip::address，ip::address\_v4 和 ip::address\_v6 类。
它们提供了相当多的函数。下面列出了最重要的几个：

* ip::address(v4\_or\_v6\_address)：这个函数把一个 v4 或者 v6 的地址转换成 ip::address。
* **ip::address::from_string(str)：这个函数根据一个 IPv4 地址（用.隔开的）或者一个 IPv6 地址（十六进制表示）创建一个地址。**
* ip::address::to_string()：这个函数返回这个地址的字符串。
* ip::address_v4::broadcast([addr, mask])：这个函数创建了一个广播地址。
* ip::address_v4::any()：这个函数返回一个能表示任意地址的地址。
* ip::address\_v4::loopback()，ip::address\_v6::loopback()：这个函数返回环路地址（为 v4/v6 协议）
* ip::host_name()：这个函数用 string 数据类型返回当前的主机名。

大多数情况你会选择用 ip::address::from_string：

```C++
ip::address addr = ip::address::from_string("127.0.0.1");
```

如果你想通过一个主机名进行连接，下面的代码片段是无用的：

```C++
// 抛出异常
ip::address addr = ip::address::from_string("www.yahoo.com");
```

## 端点

端点是使用某个端口连接到的一个地址。不同类型的 socket 有它自己的 endpoint 类，比如 ip::tcp::endpoint，ip::udp::endpoint 和 ip::icmp::endpoint。

如果想连接到本机的 80 端口，你可以这样做：

```C++
ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 80);
```

有三种方式来让你建立一个端点：

* endpoint()：这是默认构造函数，某些时候可以用来创建 UDP/ICMP socket。
* endpoint(protocol, port)：这个方法通常用来创建可以接受新连接的服务器端 socket。
* endpoint(addr, port)：这个方法创建了一个连接到某个地址和端口的端点。

例子如下：

```C++
ip::tcp::endpoint ep1;
ip::tcp::endpoint ep2(ip::tcp::v4(), 80);
ip::tcp::endpoint ep3(ip::address::from_string("127.0.0.1), 80);
```

如果你想连接到一个主机（不是IP地址），你需要这样做：

```C++
// 输出 "87.248.122.122"
io_service service;
ip::tcp::resolver resolver(service);
ip::tcp::resolver::query query("www.yahoo.com", "80");
ip::tcp::resolver::iterator iter = resolver.resolve(query);
ip::tcp::endpoint ep = *iter;
std::cout << ep.address().to_string() << std::endl;
```

















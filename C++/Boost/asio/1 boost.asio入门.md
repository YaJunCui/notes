# Boost.Asio 入门

首先，让我们先来了解一下什么是 Boost.Asio？怎么编译它？了解的过程中我们会给出一些例子。然后在发现 Boost.Asio 不仅仅是一个网络库的同时，你也会接触到 Boost.Asio 中最核心的类——io_service。

## 什么是Boost.Asio

简单来说，Boost.Asio 是一个跨平台的、主要用于网络和其他一些底层输入/输出编程的 C++ 库。

网络 API 的设计方式有很多种，但是 Boost.Asio 的方式远远优于其它的设计方式。它在 2005 年就被引入到 Boost，然后被大量 Boost 用户测试并在很多项目中使用，比如 Remobo(<http://www.remobo.com>)，可以让你创建自己的即时私有网络(IPN)的应用，libtorrent(<http://www.rasterbar.com/products/libtorrent>) 一个实现了比特流客户端的库，PokerTH (<http://www.pokerth.net>) 一个支持 LAN 和互联网对战的纸牌游戏。

Boost.Asio 在网络通信、COM 串行端口和文件上成功地抽象了输入输出的概念。你可以基于这些进行同步或者异步的输入输出编程。

```C++
read(stream, buffer [, extra options])
async_read(stream, buffer [, extra options], handler)
write(stream, buffer [, extra options])
async_write(stream, buffer [, extra options], handler)
```

从前面的代码片段可以看出，这些函数支持传入包含任意内容（不仅仅是一个socket，我们可以对它进行读写）的流实例。

作为一个跨平台的库，Boost.Asio 可以在大多数操作系统上使用。能同时支持数千个并发的连接。其网络部分的灵感来源于伯克利软件分发(BSD)socket，它提供了一套可以支持传输控制协议(TCP)socket、用户数据报协议(UDP)socket和Internet控制消息协议(IMCP)socket的API，而且如果有需要，你可以对其进行扩展以支持你自己的协议。

## 历史

Boost.Asio在2003被开发出来，然后于2005年的12月引入到Boost 1.35版本中。原作者是Christopher M. Kohlhoff，你可以通过chris@kohlhoff.com联系他。

这个库在以下的平台和编译器上测试通过：

* 32-bit和64-bit Windows，使用Visual C++ 7.1及以上
* Windows下使用MinGW
* Windows下使用Cygwin(确保已经定义 _\_USE\_232\_SOCKETS )
* 基于2.4和2.6内核的Linux，使用g++ 3.3及以上
* Solaris下使用g++ 3.3及以上
* MAC OS X 10.4以上下使用g++ 3.3及以上

它也可能能在诸如AIX 5.3，HP-UX 11i v3，QNX Neutrino 6.3，Solaris下使用Sun Studio 11以上，True64 v5.1，Windows下使用Borland C++ 5.9.2以上等平台上使用。（更多细节请咨询www.boost.org）

## 依赖

Boost.Asio依赖于如下的库：

* Boost.System：这个库为 Boost 库提供操作系统支持(<http://www.boost.org/doc/libs/1_51_0/doc/html/boost_system/index.html>)
* Boost.Regex：使用这个库（可选的）以便你重载 read\_until() 或者 async\_read\_until() 时使用 boost::regex 参数。
* Boost.DateTime：使用这个库（可选的）以便你使用 Boost.Asio 中的计时器
* OpenSSL：使用这个库（可选的）以便你使用 Boost.Asio 提供的 SSL 支持。

## 编译Boost.Asio

Boost.Asio是一个只需要引入头文件就可以使用的库。然而，考虑到你的编译器和程序的大小，你可以选择用源文件的方式来编译Boost.Asio。如果你想要这么做以减少编译时间，有如下几种方式：

在某个源文件中，添加#include “boost/asio/impl/src.hpp”（如果你在使用SSL，添加#include “boost/asio/ssl/impl/src.hpp”）
在所有的源文件中，添加#define BOOST_ASIO_SEPARATE_COMPILATION

注意Boost.Asio依赖于Boost.System，必要的时候还依赖于 Boost.Regex，所以你需要用如下的指令先编译 Boost：

```shell
bjam –with-system –with-regex stage
```

如果你还想同时编译tests，你需要使用如下的指令：

```shell
bjam –with-system –with-thread –with-date_time –with-regex –with-serialization stage
```

这个库有大量的例子，你可以连同本书中的例子一块看看。

## 重要的宏

如果设置了BOOST_ASIO_DISABLE_THREADS；不管你是否在编译Boost的过程中使用了线程支持，Boost.Asio中的线程支持都会失效。

## 同步VS异步

首先，异步编程和同步编程是非常不同的。在同步编程中，所有的操作都是顺序执行的，比如从 socket 中读取（请求），然后写入（回应）到 socket 中。每一个操作都是阻塞的。因为操作是阻塞的，所以为了不影响主程序，当在socket上读写时，通常会创建一个或多个线程来处理 socket 的输入/输出。因此，同步的服务端/客户端通常是多线程的。

相反的，异步编程是事件驱动的。虽然启动了一个操作，但是你不知道它何时会结束；它只是提供一个回调给你，当操作结束时，它会调用这个 API，并返回操作结果。对于有着丰富经验的 QT（诺基亚用来创建跨平台图形用户界面应用程序的库）程序员来说，这就是他们的第二天性。因此，在异步编程中，你只需要一个线程。

因为中途做改变会非常困难而且容易出错，所以你在项目初期（最好是一开始）就得决定用同步还是异步的方式实现网络通信。不仅 API 有极大的不同，你程序的语意也会完全改变（异步网络通信通常比同步网络通信更加难以测试和调试）。你需要考虑是采用阻塞调用和多线程的方式（同步，通常比较简单），或者是更少的线程和事件驱动（异步，通常更复杂）。

下面是一个基础的同步客户端例子：

```C++
using boost::asio;
io_service service;
ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 2001);
ip::tcp::socket sock(service);
sock.connect(ep);
```

首先，你的程序至少需要一个 io\_service 实例。Boost.Asio 使用 io_service 同操作系统的输入/输出服务进行交互。通常一个 io\_service 的实例就足够了。然后，创建你想要连接的地址和端口，再建立 socket。把 socket 连接到你创建的地址和端口。

下面是一个简单的使用Boost.Asio的服务端：

```C++
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
io_service service;
ip::tcp::endpoint ep( ip::tcp::v4(), 2001));  //listen on 2001
ip::tcp::acceptor acc(service, ep);
while (true) {
    socket_ptr sock(new ip::tcp::socket(service));
    acc.accept(*sock);
    boost::thread(boost::bind(client_session, sock));
}

void client_session(socket_ptr sock) {
    while ( true) {
        char data[512];
        size_t len = sock->read_some(buffer(data));
        if ( len > 0)
            write(*sock, buffer("ok", 2));
    }
}
```

首先，同样是至少需要一个 io\_service 实例。然后你指定你想要监听的端口，再创建一个接收器——一个用来接收客户端连接的对象。 在接下来的循环中，你创建一个虚拟的 socket 来等待客户端的连接。然后当一个连接被建立时，你创建一个线程来处理这个连接。

在 client_session 线程中来读取一个客户端的请求，进行解析，然后返回结果。

而创建一个异步的客户端，你需要做如下的事情：

```C++
using boost::asio;
io_service service;
ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 2001);
ip::tcp::socket sock(service);
sock.async_connect(ep, connect_handler);
service.run();
void connect_handler(const boost::system::error_code & ec) {
    // 如果ec返回成功我们就可以知道连接成功了
}
```

在程序中你需要创建至少一个 io_service 实例。你需要指定连接的地址以及创建 socket。

当连接完成时（其完成处理程序）你就异步地连接到了指定的地址和端口，也就是说，connect_handler 被调用了。

当 connect_handler 被调用时，检查错误代码（ec），如果成功，你就可以向服务端进行异步的写入。

注意：只要还有待处理的异步操作，servece.run() 循环就会一直运行。在上述例子中，只执行了一个这样的操作，就是 socket 的 async_connect。在这之后，service.run() 就退出了。

每一个异步操作都有一个完成处理程序——一个操作完成之后被调用的函数。 下面的代码是一个基本的异步服务端

```C++
using boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
io_service service;
ip::tcp::endpoint ep(ip::tcp::v4(), 2001));  //监听端口2001
ip::tcp::acceptor acc(service, ep);
socket_ptr sock(new ip::tcp::socket(service));
start_accept(sock);
service.run();

void start_accept(socket_ptr sock) {
    acc.async_accept(*sock, boost::bind( handle_accept, sock, _1) );
}

void handle_accept(socket_ptr sock, const boost::system::error_code &err) {
    if (err) return;
    // 从这里开始, 你可以从socket读取或者写入
    socket_ptr sock(new ip::tcp::socket(service));
    start_accept(sock);
}
```

在上述代码片段中，首先，你创建一个 io_service 实例，指定监听的端口。然后，你创建接收器 acc ——一个接受客户端连接，创建虚拟的 socket，异步等待客户端连接的对象。

最后，运行异步 service.run() 循环。当接收到客户端连接时，handle\_accept 被调用（调用 async_accept 的完成处理程序）。如果没有错误，这个 socket 就可以用来做读写操作。

在使用这个 socket 之后，你创建了一个新的 socket，然后再次调用 start_accept()，用来创建另外一个“等待客户端连接”的异步操作，从而使 service.run() 循环一直保持忙碌状态。

## 异常处理VS错误代码

Boost.Asio 允许同时使用异常处理或者错误代码，所有的异步函数都有抛出错误和返回错误码两种方式的重载。当函数抛出错误时，它通常抛出 boost::system::system_error 的错误。

```C++
using boost::asio;
ip::tcp::endpoint ep;
ip::tcp::socket sock(service);
sock.connect(ep);               // 第一行
boost::system::error_code err;
sock.connect(ep, err);          // 第二行
```

在前面的代码中，sock.connect(ep) 会抛出错误，sock.connect(ep, err) 则会返回一个错误码。

看一下下面的代码片段：

```C++
try {
    sock.connect(ep);
} catch(boost::system::system_error e) {
    std::cout << e.code() << std::endl;
}
```

下面的代码片段和前面的是一样的：

```C++
boost::system::error_code err;
sock.connect(ep, err);
if (err)
    std::cout << err << std::endl;
```

当使用异步函数时，你可以在你的回调函数里面检查其返回的错误码。异步函数从来不抛出异常，因为这样做毫无意义。那谁会捕获到它呢？

在你的异步函数中，你可以使用异常处理或者错误码（随心所欲），但要保持一致性。同时使用这两种方式会导致问题，大部分时候是崩溃（当你不小心出错，忘记去处理一个抛出来的异常时）。如果你的代码很复杂（调用很多 socket 读写函数），你最好选择异常处理的方式，把你的读写包含在一个函数 try {} catch 块里面。

```C++
void client_session(socket_ptr sock) {
    try {
        ...
    } catch ( boost::system::system_error e) {  
        // 处理错误
    }
}
```

如果使用错误码，你可以使用下面的代码片段很好地检测连接是何时关闭的：

```C++
char data[512];
boost::system::error_code error;
size_t length = sock.read_some(buffer(data), error);
if (error == error::eof)
    return; // 连接关闭
```

Boost.Asio 的所有错误码都包含在ˆ的命名空间中（以便你创造一个大型的switch来检查错误的原因）。如果想要了解更多的细节，请参照boost/asio/error.hpp头文件

## Boost.Asio中的线程

当说到Boost.Asio的线程时，我们经常在讨论：

* io_service:io_service 是线程安全的。几个线程可以同时调用 io\_service::run()。大多数情况下你可能在一个单线程函数中调用 io\_service::run()，这个函数必须等待所有异步操作完成之后才能继续执行。然而，事实上你可以在多个线程中调用 io\_service::run()。这会阻塞所有调用 io\_service::run() 的线程。只要当中任何一个线程调用了 io\_service::run()，所有的回调都会同时被调用；这也就意味着，当你在一个线程中调用 io\_service::run() 时，所有的回调都被调用了。
* socket:socket 类不是线程安全的。所以，你要避免在某个线程里读一个 socket 时，同时在另外一个线程里面对其进行写入操作。（通常来说这种操作都是不推荐的，更别说 Boost.Asio）。
* utility：就 utility 来说，因为它不是线程安全的，所以通常也不提倡在多个线程里面同时使用。里面的方法经常只是在很短的时间里面使用一下，然后就释放了。

除了你自己创建的线程，Boost.Asio 本身也包含几个线程。但是可以保证的是那些线程不会调用你的代码。这也意味着，只有调用了 io_service::run() 方法的线程才会调用回调函数。

## 不仅仅是网络通信

除了网络通信，Boost.Asio 还包含了其他的 I/O 功能。

Boost.Asio 支持信号量，比如 SIGTERM（软件终止）、SIGINT（中断信号）、SIGSEGV（段错误）等等。 你可以创建一个 signal_set 实例，指定异步等待的信号量，然后当这些信号量产生时，就会调用你的异步处理程序：

```C++
void signal_handler(const boost::system::error_code & err, int signal)
{
    // 纪录日志，然后退出应用
}
boost::asio::signal_set sig(service, SIGINT, SIGTERM);
sig.async_wait(signal_handler);
```

如果 SIGINT 产生，你就能在你的 signal_handler 回调中捕获到它。

你可以使用 Boost.Asio 轻松地连接到一个串行端口。在 Windows 上端口名称是 COM7，在 POSIX 平台上是 /dev/ttyS0。

```C++
io_service service;
serial_port sp(service, "COM7");
```

打开端口后，你就可以使用下面的代码设置一些端口选项，比如端口的波特率、奇偶校验和停止位。

```C++
serial_port::baud_rate rate(9600);
sp.set_option(rate);
```

打开端口后，你可以把这个串行端口看做一个流，然后基于它使用自由函数对串行端口进行读/写操作。比如async\_read(), write, async\_write(), 就像下面的代码片段：

```C++
char data[512];
read(sp, buffer(data, 512));
```

Boost.Asio 也可以连接到Windows的文件，然后同样使用自由函数，比如 read(), asyn_read() 等等，就像下面的代码片段：

```C++
HANDLE h = ::OpenFile(...);
windows::stream_handle sh(service, h);
char data[512];
read(h, buffer(data, 512));
```

对于POXIS文件描述符，比如管道，标准I/O和各种设备（但不包括普通文件）你也可以这样做，就像下面的代码所做的一样：

```C++
posix::stream_descriptor sd_in(service, ::dup(STDIN_FILENO));
char data[512];
read(sd_in, buffer(data, 512));
```

## 计时器

一些 I/O 操作需要一个超时时间。这只能应用在异步操作上（同步意味着阻塞，因此没有超时时间）。例如，下一条信息必须在100毫秒内从你的同伴那传递给你。

```C++
bool read = false;
void deadline_handler(const boost::system::error_code &) {
    std::cout << (read ? "read successfully" : "read failed") << std::endl;
}
void read_handler(const boost::system::error_code &) {
    read = true;
}
ip::tcp::socket sock(service);
…
read = false;
char data[512];
sock.async_read_some(buffer(data, 512));
deadline_timer t(service, boost::posix_time::milliseconds(100));
t.async_wait(&deadline_handler);
service.run();
```

在上述代码片段中，如果你在超时之前读完了数据，read 则被设置成 true，这样我们的伙伴就及时地通知我们。否则，当 deadline_handler 被调用时，read 还是 false，也就意味着我们的操作超时了。

Boost.Asio 也支持同步计时器，但是它们通常和一个简单的 sleep 操作是一样的。`boost::this_thread::sleep(500);` 这段代码和下面的代码片段完成了同一件事情：

```C++
deadline_timer t(service, boost::posix_time::milliseconds(500));
t.wait();
```

## io_service类

你应该已经发现大部分使用 Boost.Asio 编写的代码都会使用几个 io\_service 的实例。io\_service 是这个库里面最重要的类；它负责和操作系统打交道，等待所有异步操作的结束，然后为每一个异步操作调用其完成处理程序。

如果你选择用同步的方式来创建你的应用，你则不需要考虑我将在这一节向你展示的东西。
你有多种不同的方式来使用 io\_service。在下面的例子中，我们有 3 个异步操作，2 个 socket 连接操作和一个计时器等待操作：

* 有一个 io_service 实例和一个处理线程的单线程例子：

```C++
io_service service;             // 所有socket操作都由service来处理
ip::tcp::socket sock1(service); // all the socket operations are handled by service
ip::tcp::socket sock2(service);

sock1.async_connect(ep, connect_handler);
sock2.async_connect(ep, connect_handler);

deadline_timer t(service, boost::posixtime::seconds(5));
t.async_wait(timeout_handler);
service.run();
```

* 有一个 io_service 实例和多个处理线程的多线程例子：

```C++
io_service service;
ip::tcp::socket sock1(service);
ip::tcp::socket sock2(service);

sock1.async_connect(ep, connect_handler);
sock2.async_connect(ep, connect_handler);

deadline_timer t(service, boost::posixtime::seconds(5));
t.async_wait(timeout_handler);

for ( int i = 0; i < 5; ++i)
    boost::thread( run_service);

void run_service()
{
    service.run();
}
```

* 有多个 io_service 实例和多个处理线程的多线程例子：

```C++
io_service service[2];
ip::tcp::socket sock1(service[0]);
ip::tcp::socket sock2(service[1]);

sock1.async_connect(ep, connect_handler);
sock2.async_connect(ep, connect_handler);

deadline_timer t(service[0], boost::posixtime::seconds(5));
t.async_wait(timeout_handler);

for ( int i = 0; i < 2; ++i)
    boost::thread(boost::bind(run_service, i));

void run_service(int idx)
{
    service[idx].run();
}
```

首先，要注意你不能拥有多个 io_service 实例却只有一个线程。下面的代码片段没有任何意义：

```C++
for (int i = 0; i < 2; ++i)
    service[i].run();
```

上面的代码片段没有意义是因为 service[1].run() 需要 service[0].run() 先结束。因此，所有由 service[1] 处理的异步操作都需要等待，这显然不是一个好主意。

在前面的 3 个方案中，我们在等待 3 个异步操作结束。为了解释它们之间的不同点，我们假设：过一会操作 1 完成，然后接着操作 2 完成。同时我们假设每一个完成处理程序需要 1 秒钟来完成执行。

在第一个例子中，我们在一个线程中等待三个操作全部完成，第 1 个操作一完成，我们就调用它的完成处理程序。尽管操作 2 紧接着完成了，但是操作 2 的完成处理程序需要在 1 秒钟后，也就是操作 1 的完成处理程序完成时才会被调用。

第二个例子，我们在两个线程中等待 3 个异步操作结束。当操作 1 完成时，我们在第 1 个线程中调用它的完成处理程序。当操作 2 完成时，紧接着，我们就在第 2 个线程中调用它的完成处理程序（当线程 1 在忙着响应操作 1 的处理程序时，线程 2 空闲着并且可以回应任何新进来的操作）。

在第三个例子中，因为操作 1 是 sock1 的 connect，操作 2 是 sock2 的 connect，所以应用程序会表现得像第二个例子一样。线程 1 会处理 sock1 的 connect 操作的完成处理程序，线程 2 会处理 sock2 的 connect 操作的完成处理程序。然而，如果 sock1 的 connect 操作是操作 1，deadline\_timer t 的超时操作是操作 2，线程 1 会结束正在处理的 sock1 connect 操作的完成处理程序。因而，deadline_timer t 的超时操作必须等 sock1 connect 操作的完成处理程序结束（等待 1 秒钟），因为线程 1 要处理 sock1 的连接处理程序和 t 的超时处理程序。

下面是你需要从前面的例子中学到的：

* 第一种情况是非常基础的应用程序。因为是串行的方式，所以当几个处理程序需要被同时调用时，你通常会遇到瓶颈。如果一个处理程序需要花费很长的时间来执行，所有随后的处理程序都不得不等待。
* 第二种情况是比较适用的应用程序。他是非常强壮的——如果几个处理程序被同时调用了（这是有可能的），它们会在各自的线程里面被调用。唯一的瓶颈就是所有的处理线程都很忙的同时又有新的处理程序被调用。然而，这是有快速的解决方式的，增加处理线程的数目即可。
* 第三种情况是最复杂和最难理解的。你只有在第二种情况不能满足需求时才使用它。这种情况一般就是当你有成千上万实时（socket）连接时。你可以认为每一个处理线程（运行 io_service::run() 的线程）有它自己的 select/epoll 循环；它等待任意一个 socket 连接，然后等待一个读写操作，当它发现这种操作时，就执行。大部分情况下，你不需要担心什么，唯一你需要担心的就是当你监控的 socket 数目以指数级的方式增长时（超过 1000 个的 socket）。在那种情况下，有多个 select/epoll 循环会增加应用的响应时间。

如果你觉得你的应用程序可能需要转换到第三种模式，请确保监听操作的这段代码（调用 io_service::run() 的代码）和应用程序其他部分是隔离的，这样你就可以很轻松地对其进行更改。

最后，需要一直记住的是如果没有其他需要监控的操作，.run()就会结束，就像下面的代码片段：

```C++
io_service service;
tcp::socket sock(service);
sock.async_connect( ep, connect_handler);
service.run();
```

在上面的例子中，只要 sock 建立了一个连接，connect_handler 就会被调用，然后接着 service.run() 就会完成执行。

如果你想要 service.run() 接着执行，你需要分配更多的工作给它。这里有两个方式来完成这个目标。一种方式是在 connect_handler 中启动另外一个异步操作来分配更多的工作。 另一种方式会模拟一些工作给它，用下面的代码片段：

```C++
typedef boost::shared_ptr work_ptr;
work_ptr dummy_work(new io_service::work(service));
```

上面的代码可以保证 service.run() 一直运行直到你调用 useservice.stop() 或者 `dummy_work.reset(0); // 销毁 dummy_work`。

## 总结

做为一个复杂的库，Boost.Asio 让网络编程变得异常简单。构建起来也简单。而且在避免使用宏这一点上也做得很好；它虽然定义了少部分的宏来做选项开关，但是你需要关心的很少。

Boost.Asio 支持同步和异步编程。他们有很大不同；你需要在项目早期就选择其中的一种来实现，因为它们之间的转换是非常复杂而且易错的。

如果你选择同步，你可以选择异常处理或者错误码，从异常处理转到错误码；只需要在 call 函数中增加一个参数即可（错误码）。

Boost.Asio 不仅仅可以用来做网络编程。它还有其他更多的特性，这让它显得更有价值，比如信号量，计时器等等。

下一章我们将深入研究大量 Boost.Asio 中用来做网络编程的函数和类。同时我们也会学一些异步编程的诀窍。

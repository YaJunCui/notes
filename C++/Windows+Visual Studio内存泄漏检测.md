# CC++自带内存泄漏检测库

我们要记得，`new` 了之后一定要 `delete`，不然会造成内存泄露。一个内存泄漏的程序，通常不会运行太久，就可以把整个机器的内存耗光，卡死操作系统，然后就连退出调试也不行，只能重启了。如果大家使用的是 `Windows + Visual Studio` 的话，有一个简单的方法，就是在让所有 `*.cpp` 文件在预处理之后，最上面都是这几行：

```C++
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define NEW_WITH_MEMORY_LEAK_CHECKING new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new NEW_WITH_MEMORY_LEAK_CHECKING
```

方法有几个：

* 如果你的程序有多个 `cpp`，那可以把这一段写在一个大家都会直接或间接#include的头文件里面，而且最好放在最开头
* 在上面的一种情况下，如果 Visual Studio 建立的工程还是用了预编译文件头，那可以把这几行贴在stdafx.h的最上方。
* 如果你的程序只有一个cpp，那直接贴在cpp文件的最上方就好了

接下来，你需要在程序即将运行完毕的阶段，添加以下代码：

```C++
_CrtDumpMemoryLeaks();
```

你可以将其添加到主函数的 `return 0;` 语句上方。这样当你的程序运行完之后，所有 `new` 了之后没有 `delete` 的，或者 `malloc` 了之后没有 `free` 的，都会在 `Output` 窗口里面打印出每一个泄露的对象的长度、前几个字节的值、以及他们是在哪一行代码申请的内存，双击可跳。
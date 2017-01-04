# C语言中宏定义（#define）时do{}while(0)的价值

最近在新公司的代码中发现到处用到 `do{...}while(0)`，google了一下，发现Stack Overflow上早有很多讨论，总结了一下讨论，加上自己的理解,`do{...}while(0)` 的价值主要体现在：

## 1 增加代码的适应性

下面的宏定义没有使用 `do{...}while(0)`

```c
#define FOO(x) foo(x); bar(x);
```

这样宏定义，单独调用不会出现问题，例如：

```c
FOO(100)
```

宏扩展后变成：

```c
foo(x);bar(x);
```

这样调用 FOO 没有任何问题，但是 `FOO(x)` 不能放入控制语句中，例如

```c
if (condition)
    FOO(x);
else
    ...;
```

经过宏扩展后，变成了

```c
if (condition)
    foo(x);bar(x);
else
    ...;
```

这样就导致了 `语法错误`，语法错误并不可怕，在编译阶段就能发现，更致命的是他有可能导致 `逻辑错误`，这种错误编译器发现不了，一出这种问题，程序员就抓狂吧。例如：

```c
if (condition)
    FOO(x);
```

这段代码经过扩展后变成：

```c
if (condition)
    foo(x); bar(x);
```

这样一来，无论condition是true还是false，bar(x)都会被调用。有没有被这煎熬过的兄弟啊？

这时候 `do{...}while(0)` 的价值就体现出来了，修改一下 FOO 的定义

```c
#define FOO(x) do { foo(x); bar(x); } while (0)
```

这样FOO,放入控制语句中就没有问题了。

也许有人说：把foo(x);bar(x)用大括号括起来不就行了吗？比如这样定义：

```c
#define FOO(x) { foo(x); bar(x); }
```

再看下面代码：

```c
if (condition)
    FOO(x);
else
    ...;
```

扩展后：

```c
if (condition)
    {foo(x);bar(x);} ; //注意最后这个分号，语法错误
else
    ...;
```

照样语法错误；

## 2 增加代码的扩展性

我理解的扩展性，主要是宏定义中还可以引用其他宏，比如：

```c
#define FOO(x) do{OTHER_FOO(x)} while(0)
```

这样我们不用管 OTHER_FOO 是单语句还是复合语句，都不会出现问题。

## 3 增加代码的灵活性

灵活性主要体现在，我们可以从宏中break出来，例如下面的定义：

```c
#define FOO(x)  do{ \
    foo(x);  \
    if(condition(x)) \
        break; \
    bar(x) \
    ..... \
} while(0)
```

Ref: <http://www.cnblogs.com/fengc5/p/5083134.html>
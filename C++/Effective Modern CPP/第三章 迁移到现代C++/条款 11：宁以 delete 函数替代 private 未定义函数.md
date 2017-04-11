# 条款 11：宁以 delete 函数替代 private 未定义函数

如果你为其他开发者提供代码，并且你想阻止他们调用一个特定的函数，你通常不会声明这个函数。函数不声明，函数就不会被调用。太简单了！但是有时候 C++ 会帮你声明函数，并且如果你想要阻止客户调用这些函数，简单的事情就不再简单了。

这种情况只发生在“特殊的成员函数”身上，也就是，当你需要这些成员函数的时候，C++ 会自动帮你生成。条款17 详细地讨论了这些函数，但是现在，我们只考虑 copy constructor 和 copy assignment operator。这章主要讲的是，用 C++11 中更好的做法替换在 C++98 中的常用做法。然后在 C++98 中，你最想抑制的成员函数，常常是 copy 构造函数，assignment operator，或都想抑制。

在 C++98 中，阻止这些函数的方法是：把它们声明成 private 的，并且不去定义它们。举个例子，C++ 标准库中的 iostream 类层次的底层有个 class template 叫做 basic_ios。所有 istream 和 ostream 继承自（可能不是直接地）这个类。拷贝 istream 和 ostream 是不受欢迎的，因为没有一个清晰的概念规定这些操作应该做些什么。举个例子，一个 istream 对象表示一些输入值的流，有些值已经被读过了，有些值可能会在之后读入。如果一个 istream 被拷贝，那么是否有必要拷贝所有之前读过以及以后要读的值呢？处理这个问题的最简单的办法就是，定义它们为不存在的。要做到这点，只需要禁止 stream 的拷贝就行了。

为了使 istream 和 ostream 类不能拷贝，basic_ios 在 C++98 中如此实现（包括注释）：

```C++
template<class charT, class traits = char_traits<charT> >
class basic_ios　: public ios_base{
public:
    ...
private:
    basic_ios(const basic_ios&);             // not defined
    basic_ios& operator=(const basic_ios&);  // not defined
};
```

把这些函数声明为 private，可以阻止客户调用它们。故意不去定义它们意味着如果一些代码有权利访问它们（也就是，成员函数或友元类），并使用它们，那么在链接的时候，就会因为找不到函数定义而失败。

在 C++11 中，有更好的办法，它能在本质上实现所需的功能：使用“= delete”来标记 copy constructor 和 copy assignment operator，让它们成为 delete 函数。这里给出 C++11 中的 basic_ios 的实现：

```C++
template<calss charT, class trais = char_traits<charT> >
class basi_ios : public ios_base{
public:
    ...
    basic_ios(const basic_ios&) = delete;
    basic_ios& operator= (const basic_ios&) = delete;
    ...
};
```

把这些函数“删除掉”和把它们声明为 private 的不同之处看起来除了更时尚一点就没别的了，但是这里有一些实质上的优点是你没想到的。delete 函数不会被任何方式使用，所以就算在成员函数和 friend 函数中，它们如果尝试拷贝 basic_ios 对象，它们也会失败。比起 C++98（这样的错误使用在链接前无法被诊断出来），这算是一个提升。

按照惯例，delete 函数被声明为 public，而不是 private。这是有原因的。当客户代码尝试使用一个成员函数，C++ 在检查 delete 状态之前，会先检查它的可访问性。当客户代码尝试使用一个 delete private 函数，尽管函数的可访问性不会影响到它能否被使用（这个函数总是不可调用的），一些编译器只会“抱怨”出函数是 private 的。当修改历史遗留的代码，把 private-and-not-defined 成员函数替换成 delete 函数时，尤其要记得这一点（声明 delete 函数为 public 的），因为让新函数成为 public 的，将产生更好的错误消息。

比起必须要把函数声明为 private 的，delete 函数还有一个关键的优点，那就是任何函数都可以成为 delete。举个例子，假设我们有一个非成员函数，这个函数以一个整形为参数，并且返回一个 bool 表示它是否是幸运数字：

```C++
bool isLucky(int number);
```

C++ 是从 C 继承来的，这意味着很多其它类型能被模糊地视为数值类型，然后隐式转换到 int，但是一些能通过编译的调用是没有意义的：

```C++
if(isLucky('a')) ...            //'a'是一个幸运数字吗？

if(isLucky(true))...            //"true"是幸运数字吗？

if(isLucky(3.5))...             //在检查它的幸运属性前，我们是否应该把它截断为3
```

如果幸运数字必须是整形类型，我们可以阻止上面这些调用。

一种方式是用我们想过滤掉的类型创建 delete 重载函数：

```C++
bool isLucky(int number);

isLucky(char) = delete;         //拒绝char
isLucky(bool) =  delete;        //拒绝bool
isLucky(double) = delete;       //拒绝double和float
```

（你可能会感到奇怪：double 重载版本的注释中说 double 和 float 都被拒绝了。只要你记起：给出从 float 到 int 以及 float 到 double 的转换时，C++ 会更优先把 float 转换到 double，你的疑问就消散了。因此，用 float 调用 isLucky 会调用 double 版本而不是 int 版本的重载。好了，它（编译器）会先尝试调用 isLucky，事实上这个版本的重载是被 delete 的，所以在编译时就会阻止这个调用。）

尽管 delete 函数不能被使用，它们还是你程序中的一部分。因此，它们在重载解析时，它们会被考虑进去。这就是为什么只要使用上面这样的 delete 函数声明式，令人讨厌的调用就被拒绝了：

```C++
if(isLucky('a')) ...            //错误，调用一个deleted函数
if(isLucky(true))...            //错误
if(isLucky(3.5))...             //错误
```

delete 函数还有一个使用技巧（private 成员函数做不到），那就是阻止不需要的 template 实例。举个例子，假设你需要一个使用 built-in 指针的 template（第四章的建议是，比起 raw 指针，优先使用智能指针）：

```C++
template<typename T>
void processPointer(T* ptr);
```

在指针的世界中，有两种特殊的情况。一种是 void\* 指针，因为他们无法解引用，无法增加或减少，等等。另外一个就是 char\* 指针，因为他们常用来代表指向 C 风格字符串的指针，而不是指向单个字符的指针。这些特殊的情况常常需要特别处理。现在，在 processPointer template 中，让我们假设我们需要做的特殊处理是拒绝这些类型的调用。也就是不能使用 void\* 或 char\* 指针来调用 processPointer。

这很容易执行，只要把他们的实例删除（delete）掉：

```C++
template<>
void processPointer<void>(void*) = delete;

template<>
void processPointer<char>(char*) = delete;
```

现在，我们用 void\* 或者 char\* 调用 processPointer 是无效的，const void\* 和 const char\* 可能也需要是无效的，因此，这些实例也需要被删除（delete）：

```C++
template<>
void processPointer<const void>(const void*) = delete;

template<>
void processPointer<const char>(const char*) = delete;
```

并且你真想做的很彻底的话，你还需要删除（delete）掉 const volatile void\* 和 const volatile char\* 重载，然后你需要再为其他标准字符类型（std::wchar\_t, std::char16\_t 以及 std::char32\_t）做这样的工作。

有意思的是，如果你有一个函数 template 内嵌于一个 class，然后你想通过把特定的实例声明为 private（啊啦，典型的 C++98 的方法）来使它们无效，这是无法实现的，因为你无法把一个成员函数 template 特化为不同的访问等级（和主 template 的访问等级不同）。举个例子，如果 processPointer 是一个成员函数 template，内嵌于 Widget，然后你想让 void\* 指针的调用失效，尽管无法通过编译，C++98 的方法看起来像是这样：

```C++
class Widget{
public:
    ...
    template<typename T>
    void processPointer(T* ptr)
    { ... }

private:
    template<>
    void processPointer<void>(void*);   //错误
};
```

问题在于 template 特化必须写在命名空间的作用域中，而不是类的作用域中。这个问题不会影响 delete 函数，因为他们不需要不同的访问等级。他们能在class外面被删除（因此处在命名空间的作用域中）：

```C++
class Widget{
public:
    ...
    template<typename T>
    void processPointer(T* ptr)
    { ... }
    ...
};  

template<>
void Widget::processPointer<void>(void*) = delete;
```

事实上，在C++98 中，声明函数为 private 并不定义它们就是在尝试实现 C++11 的 delete 函数所实现的东西。作为一个模仿，C++98 的方法没有做到和实物（C++11 的 delete 函数）一模一样。它在 class 外面无法工作，它在 class 里面不总是起作用，就算它起作用，它在链接前可能不起作用。所以坚持使用 delete 函数把！！！

## 请记住

* 比起 private undefined function 优先使用 delete function；
* 任何函数都能被删除（deleted），包括非成员函数和 template 实例化函数。

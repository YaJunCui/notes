# 条款 08：宁以 nullptr 代替 0 和 NULL

先让我们看一些概念：字面上的 0 是一个 int，不是一个指针。如果 C++ 发现 0 在上下文中只能被用作指针，它会勉强把 0 解释为一个 null 指针，但这只是一个应变的方案。C++ 的主要规则还是把 0 视为 int，而不是一个指针。

实际上，NULL 也是这样的。NULL 的情况，在细节方面有一些不确定性，因为 C++ 标准允许它的实现不管是不是 int，只需要给出一个数值类型（比如：long）。虽然这和 0 不一样，但是事实上没有关系，因为这里的问题不是 NULL 的具体类型，而是 0 和 NULL 都不是指针类型。

在 C++98 中，这个问题造成的最主要的影响就是指针和数值类型的重载会导致意外情况。传入一个 0 或者 NULL 给这样的重载，则永远不会调用指针版本的重载：

```C++
void f(int);          //f的三个重载
void f(bool);
void f(void*);

f(0);                 //调用f(int), 而不是f(void*)
f(NULL);              //可能不能通过编译，但是通常会调用f(int),永远不会调用f(void*)
```

关于 f(NULL) 行为的不确定性反映了：关于 NULL 类型的实现（编译器）拥有自由的权利。比如，如果 NULL 被定义为 0L（也就是 long 类型的 0），那么对函数的调用引起歧义，因为把 long 转换为 int，bool，void* 都是同样可行的。关于调用 f 的一个有趣的事情是源代码表面上的意义（我使用 NULL---null 指针，调用 f）和它实际的意义（我使用某种整形---不是一个指针，调用 f）之间的矛盾。这种反直觉的行为导致了 C++98 程序员指导方针让我们避免重载指针类型和整形类型。这个方针在 C++11 中仍然有效，因为，就算有这个条款的建议，尽管 nullptr 是更好的选择，有些开发者还是会继续使用 0 和 NULL。

nullptr 的优点不只是它不是一个整形类型。老实说，它也不是一个指针类型，但是你能把它想成是一个指向所有类型的指针。nullptr 的实际类型是 std::nullptr\_t，在一个完美的循环定义（circular definition）中，std::nullptr\_t 被定义为 nullptr 的类型。std::nullptr\_t 类型能隐式转换到所有的原始（raw）指针类型，就是这个原因，让 nullptr 表现得像它是所有类型的指针。

用 nullptr 调用 f 的重载函数，实际会调用 void* 版本的函数（也就是指针版本的重载），因为 nullptr 不能被视为任何数值类型：

```C++
f(nullptr);         //调用f(void*)版本的重载函数
```

因此使用 nullptr 来代替 0 和 NULL 避免了意外的重载解析，但是这不是他唯一的优点。它还能提升代码的可读性，尤其是在涉及 auto 变量时。举个例子，假设你遇到下面的代码：

```C++
auto result = findRecord(/* arguments*/);

if(result == 0){
    ...
}
```

如果你碰巧不知道（或者很难找出）findRecord 返回的类型，这里对于 result 是指针类型还是整形类型就不明确了。总之，0（用来和result进行比较）可以是两个中的任意一个。但是，如果你看到下面的代码：

```C++
auto result = findRecord(/* arguments*/);

if(result == nullptr){
    ...
}
```

这里将没有歧义：result 肯定是一个指针类型。

当 template 涉及后，nullptr 将变得更加耀眼。假设你有一些函数，它们只能在适当的互斥量被锁住之后才能调用。每个函数使用不同类型的指针：

```C++
int     f1(std::shared_ptr<Widget> spw);
double  f2(std::unique_ptr<Widget> upw);
bool    f3(Widget* pw);
```

想要传入 null 指针来调用函数的代码看起来像这样：

```C++
std::mutex f1m, f2m, f3m;

using MuxGuard =                //C++11 的typedef，看Item 9
    std::lock_guard<std::mutex>;
...
{
    MuGuard g(f1m);             //锁住f1的互斥量
    auto result = f1(0);        //传入0作为null指针给f1
}                               //解锁互斥量
...
{
    MuGuard g(f2m);             //锁住f2的互斥量
    auto result = f2(NULL);     //传入NULL作为null指针给f2
}                               //解锁互斥量
...
{
    MuGuard g(f3m);             //锁住f3的互斥量
    auto result = f3(nullptr);  //传入nullptr作为null指针给f3
}                               //解锁互斥量
```

没有用 nullptr 调用前面两个函数是令人忧伤的，但是代码能正常工作，这一点很重要。然而，代码中重复的模式（加锁，调用函数，解锁）不仅令人忧伤，这更是令人不安的。这种源代码的重复问题就是 template 被设计来解决的其中一种，所以让我们模板化这个模式：

```C++
template<typename FuncType, typename MuxType, typename PtrType>
auto lockAndCall(FuncType func,
                 MuxType& mutex,
                 PtrType ptr) -> decltype(func(ptr))
{
    MuxGuard g(mutex);
    return func(ptr);
}
```

如果这个函数的返回类型（auto ... ->decltype(func(ptr))）轰击了你的大脑，请问你的大脑经历并赞成过 条款03（解释了发生了什么）吗？你也能看到C++14版本的代码，返回类型能被简化成一个简单的decltype(auto)：

```C++
template<typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func,   //C++14
                           MuxType& mutex,
                           PtrType ptr)
{
    MuxGuard g(mutex);
    return func(ptr);
}
```

给出 lockAndCall 模板（每个版本）的调用函数，能这么写：

```C++
auto result1 = lockAndCall(f1, f1m, 0);         //错误
...
auto result2 = lockAndCall(f2, f2m, NULL);      //错误
...
auto result3 = lockAndCall(f3, f3m, nullptr);   //正确
```

很好，它们能这么写，但是，正如注释说的，前面两种情况，代码无法通过编译。第一种调用情况的问题是当 0 被传入 lockAndCall时，template 类型推导会找出它的类型。0 的类型过去是，现在是，未来也永远是 int，所以这就是在实例化的 lockAndCall 中，ptr 参数的类型。不幸地，这意味着在 lockAndCall 中，调用 func 时，会传入一个 int 类型，然而它和 f1 需要的std::shared\_ptr\<Widget\> 参数不兼容。在 lockAndCall 调用中，尝试用传入 0 来代替一个 null 指针，但是实际上传入的是普通的 int。尝试传入一个 int 作为 std::shared\_ptr\<Widget\> 给 f1 将产生类型错误。对于用 0 调用 lockAndCall 会失败，是因为在 template 中，一个 int 被传入一个需要 std::shared\_ptr\<Widget\> 的函数。（译注：理解起来很简单，这里没有 int 到 指针类型的转换，只有 0 视为指针这一情况。所以调用会失败。）

对于调用涉及 NULL 的分析在本质上和 0 是一样的。当 NULL 被传入 lockAndCall 时，一个整形类型被推导为 ptr 参数的类型，并且当一个 int 或一个类似 int 类型的参数被传入 f2（需要一个 std::unique\_ptr\<Widget\> 类型的参数）时，类型错误就发生了。

作为对比，当调用涉及 nullptr 时没有问题。当 nullptr 被传入 lockAndCall 时，ptr 的类型被推导为 std::nullptr\_t。当 ptr 被传入 f3 时，这里发生了一个从 std::nullptr\_t 到 Widget* 的隐式转换，因为 std::nullptr\_t 能隐式转换到任何类型的指针。

事实上，当你想使用一个 null 指针时，对于 0 和 NULL，template 类型推导推导出“错误”的类型（也就是，它们的“对的"类型本应该是代表着一个 null 指针的），这是用 nullptr 代替 0 或 NULL 最重要的原因。使用 nullptr，template 不会面临特殊的挑战。结合 nullptr 不会遭受意外的重载解析（0 和 NULL 很可能遭受）的事实，情况很清晰了。当你想使用一个 null 指针的时候，使用 nullptr，而不是 0 或 NULL。

## 请记住

* 宁以 nullptr 代替 0 和 NULL
* 避免重载整形类型和指针类型。

# 条款 03：理解 decltype

decltype 是一个奇怪的东西。给出一个名字或者一个表达式，decltype可以告诉你名字或表达式的类型。大多情况下，他告诉你的就是确实你想的那样。但是偶尔，他会提供一个脱离你想象的结果，这将导致你必须去找一本参考书或者去在线Q&A网站寻求答案。

我们从一般情况（没有意外的结果）开始。对比 template 和 auto 的类型推导，decltype 模仿你给的名字或表达式：

```C++
const int i = 0;            //decltype(i)是const int

bool f(cosnt Widget& w);    //decltype(w)是const Widget&，decltype(f)是bool(const Widget&)

struct Point {
    int x, y;               //decltype(Point::x)是int
};                          //decltype(Point::y)是int

Widget w;                   //decltype(w)是Widget

if(f(w))...                 //decltype(f(w))是bool

template<template T>
class vector{
public:
    T& operator[](std::size_t index);
};

vector<int> v;              //decltype(v)是vector<int>
if(v[0] == 0)...            //decltype(v[0])是int&
```

看吧，没有任何的意外之处。

**在 C++11 中，也许 decltype 最主要的用途就是用来声明返回值类型依赖于参数类型的函数模板。** 举个例子，假设你要写一个函数，这个函数需要一个支持下标操作（”[]”）的容器，然后根据下标操作来识别出用户，函数的返回值类型应该和下标操作的类型相同。

以 T 为元素类型的容器，operator[] 操作通常返回 T&。例如，这是 std::deque 的情况，也是 std::vector 的大多数情况。然而，对于 `std::vector<bool>`，operator[] 操作不返回 bool&。取而代之的，它返回一个表示同样值的新对象，对于这种情况的讨论将放在 条款06，但是在这里，重点是：operator[] 函数返回的类型取决于容器的类型。

decltype 让表达式变得简单。我们将写下第一个 template，展示如何用 decltype 计算返回值。template 可以进一步精炼，但是我们先写成这样：

```C++
template<typename Container,typename Index>
auto authAndAccess(Container& c, Index i)   //需要精炼
    ->decltype(c[i])
{
    authenticateUser();
    return c[i];
}
```

函数前面的那个 auto 的使用没有做任何类型的推导。当然了，这标志着 C++11 使用了返回值类型后置的语法。也就是，函数的返回值类型将根据参数列表之后（”->”后面）的参数进行推导。后置的返回类型的优点，就是函数的参数能用来确定返回值类型。例如，在authAndAccess() 中，我们用 c 和 i 来明确函数的返回值类型。按传统的使用方法，我们让返回值类型处于函数名的前面，那么 c 和 i 就不能使用了，因为解析到这时，他们还没被声明出来。

使用这个声明方式，就和我们的需求一样，authAndAccess() 根据我们传入的容器，以这个容器的 operator[] 操作返回的类型来作为它自身的返回值类型。

C++11 允许我们推导一些比较简单的 lambdas 表达式的返回类型，并且在 C++14 中，把这种推导扩张到了所有的 lambdas 表达式和所有的函数中，包括那些有多条语句的复杂的函数。在 authAndAccess() 中，这意味着在 C++14 中，我们能忽略返回值类型，只需使用 auto 即可。在这样的声明形式下，auto 意味着类型推导将会发生。尤其是，它意味着编译器将会根据函数的实现来推导函数的返回值类型。

```C++
template<typename Container, typename Index>
auto authAndAccess(Container& c, Index i)   //需要精炼
{
    authenticateUser();
    return c[i];
}
```

条款02 解释了一个返回 auto 的函数，编译器采用 template 的类型推导规则。在这种情况下，这是有问题的。就像我们讨论的那样，对于大多数容器，operator[] 返回的是 T&，但是 条款01 解释了在 template 类型推导中，表达式的引用属性会被忽略（情况3），考虑下这对于客户代码意味着什么：

```C++
std::deque<int> d;
authAndAccess(d, 5) = 10;              //这会返回d[5]，然后把10赋给它。这会出现编译错误（给右值赋值）
```

这里，d[5] 返回一个 int&，但是对 authAndAccess() 的 auto 返回类型的推导将去掉引用属性，这产生了一个 int 类型，int 是函数的返回值类型，是一个右值，然后这段代码尝试给一个右值 int 赋值一个 10.这在 C++ 中是禁止的，所以代码无法编译。

为了让 authAndAccess() 能像我们希望的那样进行工作，我们需要对返回值类型使用 decltype 类型推导，也就是明确 authAndAccess() 应该返回表达式 c[i] 返回的类型。C++ 的规则制定者，预料到了在一些类型需要推测的情况下，用 decltype 类型推导规则来推导的需求，所以在 C++14 中，通过 decltype(auto) 类型说明符来让之成为可能。这一开始看起来可能有点矛盾的东西（decltype 和 auto）确实完美地结合在一起：**auto 指出类型需要被推导，decltype 明确推导时使用 decltype 推导规则，而不是 auto 的推导规则。** 因此我们像这样能写出 authAndAccess() 的代码：

```C++
template<typename Container, typename Index>
decltype(auto)                        //使用decltype的推导规则，即保留引用和CV限定符
authAndAccess(Container& c, Index i)  //需要精炼
{
    authenticateUser();
    return c[i];
}
```

现在 authAndAccess() 能真实地返回 c[i] 返回的类型了。尤其是对于一些 c[i] 返回一个 T& 的情况，authAndAccess() 也会返回一个 T&，并且在不寻常的情况下，当 c[i] 返回一个对象，authAndAccess() 也会返回一个对象。

decltype(auto) 的使用不止局限于函数返回类型，当你对正在初始化的表达式使用 decltype 类型推导规则时，它也能很方便地用在变量的声明上：

```C++
Widget w;
cosnt Widget& cw = w;

auto myWidget1 = cw;                //myWidget1的类型是Widget
decltype(auto) myWidget2 = cw;      //myWidget2的类型是const Widget&
```

我知道现在有两件事困扰着你，一个是我上面提到却没有讨论的对于 authAndAccess() 的精炼，现在让我们处理它：

再一次看一下 C++14 版本的 authAndAccess() 的声明：

```C++
template<typename Container, typename Index>
decltype(auto) authAndAccess(Container& c, Index i);
```

容器以非 cons t左值引用（lvalue-reference-to-non-cosnt）的方式传入，因为一个对元素的引用允许客户更改容器，但是这意味着我们不能传一个 rvalue 容器给这个函数。rvalue 不能和 lvalue 引用绑定（除非他们是 const 左值引用（lvalue-references-to-const），但是这里不是）。

公认地，传一个 rvalue 容器给 authAndAccess() 是很罕见的情况。一个 rvalue 容器是一个临时对象，它在 authAndAccess() 的调用语句结束的时候就会被销毁，这意味着，对一个右值容器中元素的引用（尤其是在authAndAccess()将要返回时）在语句结束时会产生未定义的结果。但是，传递一个临时变量给 authAndAccess() 还是有意义的。一个客户可能简单地想产生临时容器中元素的一份拷贝，举个例子：

```C++
std::deque<std::string> makeStringDeque();

auto s = authAndAccess(makeStringDeque(), 5);  //拷贝makeStringDeque()[5]的元素
```

为了支持这样的使用，意味着我们需要修改 authAndAccess() 的声明，让它能同时接受 lvalues 和 rvalues。重载可以很好的工作（一个接受lvalue引用参数的函数，一个接受rvalue引用参数的函数），但是这样的话会我们需要维护两个函数。一个避免这样的方法是使用一个引用参数（universal引用），它能同时接受 lvalues 和 rvalues，在 条款24 中会解释 universal引用具体做了什么。因此 authAndAccess() 能声明成这样：

```C++
template<typename Container, typename Index>
decltype(auto) authAndAccess(Container&& c, Index i);
```

在这个template中，我们不知道容器的类型，所以，这意味着我们同样不知道容器对象使用的索引类型。对于未知对象使用传值（pass-by-value）方式通常会因为没必要的拷贝而对效率产生很大的影响，以及对象切割（条款41）的问题，并会被我们的同事嘲笑，但是在容器索引的使用上，跟随标准库（比如，std::string，std::vector以及std::deque的operator[]）的脚步看起来是合理的。所以我们将坚持以传值（pass-by-value）的方式使用它。

但是，我们需要使用 std::forward 提升 template 对 universal 引用的实现，让它符合 条款25 的告诫。

```C++
template<typename Container, typename Index>
decltype(auto)
authAndAccess(Container&& c, Index i)
{
    authnticateUser();
    return std::forward<Container>(c)[i];
}
```

这个例子应该会做到所有我们想要的事情了，但是它需要 C++14 的编译器。如果你没有，你将需要使用 C++11 版本的 template。除了你需要自己明确返回值类型外，它和 C++14 的版本是一样的：

```C++
template<typename Container, typename Index>
auto
authAndAccess(Container&& c, Index i)
    -> decltype(std::forward<Container>(c)[i])
{
    authnticateUser();
    return std::forward<Container>(c)[i];
}
```

另一个可能纠缠你的问题是我在一开始的评论，我说 decltype 在大多数情况下回产生你想要的类型，几乎不会带来意外。老实说，你不太可能遇到这些例外规则除非你是一个重大的库的实现者。

为了完全理解 decltype 的行为，你必须让你自己熟悉一些特殊情况。大多数这些情况都太隐晦了所以不会确切地在书中讨论，但是为了看清楚 decltype 的内部情况以及他的使用需要借助这种情况。

应用 decltype 来产生一个名字（name）的类型，如果名字（name）是左值表达式，不影响 decltype 的行为。比起名字（names）lvalue 表达式更加复杂，然而 decltype 保证推导的类型总是lvalue引用。那就是，一个lvalue表达式除了 name 会推导出 T，其他情况 decltype 推导出来的类型就是 T&。这很少有影响，因为大多数 lvalue 表达式本质上包含一个 lvalue 引用的修饰符。比如函数返回 lvalues 时，它总是返回左值引用。

这里有一个隐含的行为值得你去意识到，在

```C++
int x = 0;
```

中 x 是一个变量的名字，所以 decltype(x) 是 int，但是如果把 x 包装在括号中—”(x)”—将产生一个比名字更复杂的表达式。作为一个名字，x 是一个 lvalue，并且 C++ 定义表达式(x)也是一个左值。因此，decltype((x)) 是 int&。把括号放在 name 的两旁将改变 decltype 推导出来的类型。

在 C++11 中，没什么好奇怪的，但是结合 C++14 对 decltype(auto) 的支持，这意味着你在函数中写的返回语句将影响到函数类型的推导：

```C++
decltype(auto) f1()
{
    int x = 0;
    return x;                    //decltypex(x)是int，所以f1返回int
}

decltype(auto) f2()
{
    int x = 0;
    return (x);                  //decltype((x))是int&，所以f2返回int&
}
```

记住 f2 不仅返回值和 f1 不同，而且，它返回了一个局部变量。这是一种把你推向未定义行为的陷阱代码。

最重要的教训就是，在使用 decltype(auto) 的时候小心再小心。在表达式中，对类型的推导看起来无关紧要的细节能影响到 decltype(auto) 的推导。为了保证类型的推导和你想的一样，请使用 条款04 描述的方法。

同时，不要忽视大局。当然，decltype（包括单独使用以及和auto一起使用）可能偶尔产生出意外的类型，但是那不是通常的情况。通常，decltype 产生你想要的类型。当 decltype 应用在 name 时，它总产生你想要的情况，在这情况下，decltype 就像听起来那样：它推导出name的声明类型。

## 你要记住的事

* decltype 大多数情况下总是不加修改地产出变量和表达式的类型
* 对于 T 类型的 lvalue 表达式，decltype总是产出 T&。
* C++14 支持 decltype(auto)，和 auto 一样，它从一个初始化器中推导类型，只不过使用的是 decltype 类型推导规则。

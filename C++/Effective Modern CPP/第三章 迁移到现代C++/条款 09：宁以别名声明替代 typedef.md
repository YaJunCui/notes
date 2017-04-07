# 条款 09：宁以别名声明替代 typedef

我确信我们都同意使用 STL 容器是一个好主意，并且我希望在 条款18 中能让你相信使用 std::unique\_ptr 也是一个好主意，但是我猜想，我们中没有任何一个人想多次写这样的类型：“std::unique\_ptr<std::unordered\_map<std::string, std::string>>”。光是想想就感觉，得“腕管综合症”的风险会增加。

为了避免这样的医学上的悲剧很简单，引进 typedef：

```C++
typedef std::unique_ptr<std::unordered_map<std::string, std::string>> UPtrMapSS;
```

但是 typedef 太太太 C++98 了。当然，它们能在 C++11 中工作，但是 C++11 也提供了别名声明（alias declaration）：

```C++
using UPtrMapSS = std::unique_ptr<std::unordered_map<std::string, std::string>>;
```

给出的 typedef 和别名声明（alias declaration）做的是同样的事情，这值得我们去考虑这里是否有什么科学的原因来让我们更偏爱其中一个。

是的，但是在我说明之前，我想谈一下别的：很多人发现在涉及函数指针的时候，别名声明（alias declaration）更好接受一点：

```C++
//FP 是指向一个函数的指针的别名，这个函数以一个int和一个
//const std::string&为参数，不返回任何东西。
typedef void (*FP)(int, const std::string&);    //typedef

//和上面同样的意义
using FP = void(*)(int, const std::string&);    //别名声明
```

当然，这两个都很容易接受，并且不管怎么说，很少人需要花费大量的时间学习使用函数指针类型的别名，所以这几乎不是一个强硬的理由来让我们选择别名声明（alias declaration）替换typedef。

但是强硬的理由是存在的：template。尤其是，别名声明（alias declaration）能模板化（我们称之为别名模板（alias template）），但是 typedef 不能这么做。这给了 C++11 程序员一个简单的机制来表达在 C++98 中必须使用嵌套在模板化 struct 中的 typedef 来做的事情。举个例子，考虑定义一个链表的别名，这个链表使用自定义的分配器（allocator）MyAlloc。使用别名声明（alias declaration），这就是小菜一碟：

```C++
//MyAllocList<T>就是std::list<T, MyAlloc<T>>的别名
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;

MyAllocList<Widget> lw;                     //客户代码
```

使用 typedef，你就有麻烦了：

```C++
//MyAllocList<T>::type就是std::list<T, MyAlloc<T>>的别名
template<typename T>
struct MyAllocList{
    typedef std::list<T, MyAlloc<T>> type;
};

MyAllocList<Widget>::type lw;              //客户代码
```

如果你想在 template 中使用 typedef 来达到使用 template 参数作为链表的模板类型参数来创建别名的目的，这将变得更糟糕，你必须在 typedef 前面加上 typename：

```C++
//Widget中有一个MyAllocList<T>作为成员变量
template<typename T>
class Widget{
private:
    typename MyAllocList<T>::type list;
    ...
};
```

这里，MyAllocList\<T\>::type 指的是一个依赖于 template 的参数（T）的类型。MyAllocList\<T\>::type 因此是一个*依赖类型*（dependent type），并且 C++ 的众多“可爱”的规则中的一个就是，在依赖类型前面必须加 typename。

如果 MyAllocList 被定义为别名模板（alias template），对于 typename 的需求就消失了（笨重的“::type”后缀也消失了）：

```C++
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;

template<typename T>
class WidgetP
private:
    MyAllocList<T> list;                //没有"typename"，没有"::type"
    ...
};
```

对于你，MyAllocList\<T\>(也就是，使用alias template)可能看起来和 MyAllocList\<T\>::type (也就是，使用嵌套typedef)一样，也依赖于 template 参数（T），但是你不是编译器，当编译器处理 Widget template，并且碰到 MyAllocList\<T\> 的使用时（也就是，使用 alias template时），它知道 MyAllocList\<T\>就是类型的名字，因为 MyAllocList 是一个 alias template：它肯定代表一个类型。因此，MyAllocList\<T\> 是一个非依赖类型（non-dependent type），并且它既不需要也不允许使用 typename。

另一方面，当编译器在 Widget template 中看到 MyAllocList\<T\>::type(也就是，使用嵌套typedef)时，编译器不肯定它就代表一个类型，因为编译器不知道这里是否有一个特化的 MyAllocList，它的 MyAllocList\<T\>::type 代表的不是一个类型。这听起来很疯狂，但是对于这个可能性，不要指责编译器。我们人类知道怎么产生这样的代码。

举个例子，一些误入歧途的灵魂可能像这样被制造出来：

```C++
class Wine{...};

template<>                  //特化的MyAllocList
class MyAllocList<Wine>{    //特化的类型是Wine
private:
    enum class WinType      //关于"enum class"的信息
    { White, Red, Rose};    //请看Item 10

    WineType type;          //在这个类中，type是一个成员变量
    ...
};
```

就像你看到的，MyAllocList\<Wine\>::type 不是一个类型，如果 Widget 用 Wine 来实例化，Widget template 中的 MyAllocList\<T\>::type 指向一个成员变量，而不是类型。因此，在 Widget template 中，MyAllocList\<T\>::type 是否代表一个类型需要依赖于 T 是什么，并且这也就是为什么如果你认为这是一个类型，编译器还是坚持让你在前面使用 typename。

如果你做过任何模板元编程（template metaprogramming （TMP）），你肯定碰到过这样的需求：使用 template 类型参数创造修改后的类型。举个例子，对于给定的类型 T，你可能想要去掉 T 上的 const 或引用属性。比如，你可能想要把 const std::string& 变成 std::string。或者你可能想要增加 const 给一个类型，或者把它变成一个左值引用。比如，把 Widget 变成 const Widget 或者 Widget&。（如果你不知道任何TMP，那太糟糕了，因为如果你想要成为一个真正厉害的 C++ 程序员，你至少需要熟悉 C++ 在这方面（TMP）的基础。在 条款23 和 条款27 中，你能看到 TMP 的例子，包含我提及的类型转换）。

在头文件\<type_traits\>的各种模板中，C++11 提供给你一些工具，让你以 type traits 的形式来做这些转换。头文件中有许多 type traits，但不是全都用来做类型转换的，但是它提供一些可预测的接口，给出一个你想转换的类型 T，结果的类型就是 std::transformation\<T\>::type：

```C++
std::remove_const<T>::type          //用 const T 产生 T
std::remove_reference<T>::type      //用 T& 或 T&& 产生 T
std::add_lvalue_reference<T>::type  //用 T 产生 T&
```

注释只是总结了这些转换做了什么，所以使用时不要过于随便。在工程中使用它们前，我知道你会先看一下参考手册的。

不管怎么说，我的目的不是给你一个 type traits 的引导，而是强调这些转换在使用时需要在后面写上“::type”。如果你在 template 中把它们应用于 template 类型参数（你常常需要在代码中这样使用），你还要在前面加上 typename。原因是，在 C++11 的 type traits 中，这些语法使用嵌套于模板化 struct 中的 typedef 使用。好的，就是因为这样的别名实现的技术，我想让你知道它不如别名模板（alias template）！

C++11 中这么实现是有历史原因的，但是我们不去讨论它（我保证这很无聊），因为 C++ 标准委员会很迟才认识到 alias templates 才是最好的做法，并且对于 C++11 中的所有转换，他们在 C++14 中包含了所有的 alias template 版本。所有的别名都有一样的形式：每个 C++11 的转换 std::transformation\<T\>::type，在 C++14 中同样有 alias template 以 std::transformation_t 命名。例子将解释我说的：

```C++
std::remove_const<T>::type          //C++11: const T -> T
std::remove_const_t<T>              //C++14 等价的操作

std::remove_reference<T>::type      // C++11: T&/T&& → T
std::remove_reference_t<T>          // C++14 等价的操作

std::add_lvalue_reference<T>::type  // C++11: T → T&
std::add_lvalue_reference_t<T>      // C++14 等价的操作
```

C++11 版本的转换在 C++14 中仍然有效，但是我不知道你有什么理由去使用它们。甚至如果你没有使用 C++14，自己写一份 alias template 就像玩一样。只需要 C++11 的语言特性，甚至连小孩都能模仿这种模式，是吧？如果你碰巧有一份 C++14 标准的电子稿，将变得更加简单，因为要做的事就只有拷贝和粘贴。这里，我给你一个开头：

```C++
template <class T>
using remove_const_t = typename remove_const<T>::type;

template <class T>
using remove_reference_t = typename remove_reference<T>::type;

template <class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
```

看到了吗，没有比这更简单的事了。

## 请记住

* typedef 不支持模板化，但是别名声明（alias declaration）支持。
* alias templates 避免了“::type”后缀，以及在template中“typename”前缀（当代表一个类型时）的使用。
* C++14 提供所有 C++11 type traits 转换的 alias templates 版本。

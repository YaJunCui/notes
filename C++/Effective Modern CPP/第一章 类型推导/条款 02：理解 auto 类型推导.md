# 条款 02：理解 auto 类型推导

如果你已经读过 条款01 的模板类型推导，你已经知道大部分关于 auto 类型推导的知识了，因为，除了一种奇怪的情况外，auto 类型推导和 template 类型推导是一样的。但是这怎么可能呢？template 类型推导涉及模板和函数以及参数，但是 auto 没有处理这些东西！

是这样的，但是这没关系。从 template 类型推导到 auto 类型推导有一个直接的映射关系。这里有一个直接的算法来从 template 类型推导转换到 auto 类型推导。

在 条款01 中，template类型推导是解释通用函数模板：

```C++
template<typename T>
void f(ParamType param);
```

并且这样简单调用：

```C++
f(expr);
```

对于 f 的调用，编译器用 expr 来推导 T 和 ParamType 的类型。

当一个变量用 auto 来声明时，auto 扮演着的角色相当于 template 中的 T，并且变量的类型相当于 ParamType。直接举个例子会更简单一些：

```C++
auto x = 27;
```

这里，x 的类型就是 auto 自身，另一方面，在这个声明中：

```C++
cosnt auto cx = x;
```

类型是 const auto，而：

```C++
cosnt auto& rx = x;
```

的类型是 const auto&。为了推导 x，cx，rx的类型，编译器表现地好像这里有 template 的声明，并且用初始化表达式调用相应的 template：

```C++
tamplate<typename T>        //概念上的 x 的template类型推导
void func_for_x(T param);

func_for_x(27);             //概念上的调用：param的类型推导, 结果将会是 x 的类型

template<typename T>
void func_for_cx(const T param);

func_for_cx(x);

template<typename T>
void func_for_rx(cosnt T& param);

func_for_rx(x);
```

就像我说的，auto 的类型推导，只有一个例外（我们马上就会讨论），其他的和 template 的类型推导是一样的。

条款01 基于 ParamType 的不同情况，把 template 的类型推导分成三种情况。而使用 auto 来声明变量，变量类型取代了 ParamType，所以这里也有三种情况：

* 情况1：变量类型是指针或引用，但是不是 universal 引用。
* 情况2：变量类型是 universal 引用。
* 情况3：变量类型不是指针也不是引用。

我们已经看过情况1和情况3的例子了：

```C++
auto x = 27;        //情况3
const auto cx = x;  //情况3
const auto& rx = x; //情况1
```

你可以把 情况2 想象成这样：

```C++
auto&& uref1 = x;   //x 是 int，并且是左值，所以 uref1 的类型是 int&
auto&& uref2 = cx;  //cx 是 const int，并且是左值，所以 uref2 的类型是 const int&
auto&& uref3 = 27;  //27 是 int，并且是右值，所以 uref3 的类型是 int&&
```

条款01 讨论了当类型是 non-reference 时，数组和函数退化成指针的情况，这样的情况也发生在 auto 类型推导中：

```C++
const char name[] = "R. N. Brigs";  //name 的类型是 const char[13]
auto arr1 = name;                   //arr1 的类型是 const char*
auto& arr2 = name;                  //arr2 的类型是 const char()[13]
void someFunc(int, double);         //声明一个函数，类型是 void(int, double)
auto func1 = someFunc;              //func1 的类型是 void(*)(int，double)
auto& func2 = someFunc;             //func2 的类型是 void()(int, double)
```

就和你看到的一样，auto 类型推导和 template 类型推导一样，他们本质上是硬币的两边。

只有一种情况下，他们是不同的。我们将要开始观察这个情况，如果你想用 27 声明一个 int 变量，C++98 允许你使用两种不同的语法：

```C++
int x1 = 27;
int x2(27);
```

C++11中，通过它对标准初始化的支持，增加了这些操作：

```C++
int x3 = {27};
int x4{27}；
```

总之，四种语法都产生一个结果：一个值为 27 的 int 变量。

但是根据 条款05 的解释，声明变量时，用 auto 来替换确切的类型有几点好处，所以在上面的变量声明中，用 auto 来替换 int 是有好处的。直接的文本替换可以产生这样的代码：

```C++
auto x1 = 27;
auto x2(27);
auto x3 = {27}；
auto x4{27};
```

这些声明都能编译，但是在用auto替换后，它们拥有了不同的解释。前两条语句确实声明了值为 27 的 int 变量。然而，后面两条语句，声明的变量类型为带有值为 27 的 std::initializer_list ！

```C++
auto x1 = 27;   //类型是int， 值是27
auto x2(27);    //同上
auto x3 = {27}; //类型是 initializer_list<int>, 值是 {27}
auto x4{27};    //同上
```

产生这样的结果是由于 auto 的特殊的类型推导规则。当用初始化列表的形式来初始化 auto 声明的变量时，推导出来的类型就是 std::initializer_list,下面这样的代码是错误的：

```C++
auto x5 = {1, 2, 3.0};    //错误！不能推导 initializer_list<T> 中 T 的类型
```

就像注释里说的，这种情况下的类型推导将会失败，但是你需要知道这里其实产生了两种类型推导。一种就来自 x5 的auto 类型推导。因为 x5 的初始化在花括号中，x5 就被推导为 std::initializer\_list。但是，std::initializer\_list 是一个 template。用 T 来实例化 std::initializer\_list 意味着 T 的类型也必须被推导出来。这里发生的推导属于第二种类型推导：template 类型推导。在这个例子中，这个推导失败了，因为初始化列表中的值不是同种类型的。

auto 类型推导和 template 类型推导唯一的不同之处，就是对待初始化列表的不同做法。当用初始化列表来声明auto 类型的变量时，推导出来的类型是 std::initializer_list 的一个实例。但是如果传入同样的初始化列表给 template，类型推导将会失败，并且代码编译不通过：

```C++
auto x = {11, 23, 9};    //类型是 std::initializer_list<int>

template<typename T>
void f(T param);

f({11, 23, 9});          //错误！不能推导 T 的类型
```

然而，如果你明确 template 的参数为 std::initializer_list，template类型推导将成功推导出 T：

```C++
template<typename T>
void f(std::initializer_list<T> initList);

f({11, 23, 9})  //T 被推导为 int，initList 的类型是 std::initializer_list<int>
```

所以在 auto 和 template 的类型推导的唯一不同之处就是，auto 假设初始化列表为 std::initializer_list，但是 template 类型推导不这么做。

你可能想知道为什么 auto 类型推导对初始化列表有这么一个特别的规则，但是 template 类型推导却没有。我也想知道，可悲的是，我没有找到一个令人信服的解释。但是规则就是规则，并且这意味着你必须记住，如果你用 auto 声明一个变量并且用初始化列表来初始化它，那么被推导出来的类型就是 std::initializer\_list。如果你遵守标准初始化的哲学——初始值在花括号中的思想理所当然的，把上述推导规则记住是极为重要的。C++11中的一个典型的错误就是，当你想声明一个其它变量时，意外地声明了一个 std::initializer_list 变量。这个陷阱导致了，有些开发者只在他们必须时才用大括号来初始化变量（什么时候才是必须的情况将在 条款07 中讨论）。

对于 C++11 来说，故事已经完结了，但是对 C++14 来说，故事还将继续。C++14 允许 auto 作为函数的返回值（见 条款03），并且在 lambdas 表达式中可能用 auto 来作为形参类型。然而，auto 的这些用法使用的是 template 类型推导规则，而不是 auto 类型推导规则。所以返回一个初始化列表给 auto 类型的函数返回值将不能通过编译：

```C++
auto createInitList()
{
    return {1, 2, 3};            //错误！不能推导{1，2，3}的类型
}
```

在 C++14 中，这对于作为 lambdas 表达式的参数声明的 auto 类型同样适用：

```C++
std::vector<int> v;
auto resetV = [&v](const auto& newValue){
                v = newValue;
              };
resetV({1, 2, 3});  //错误！不能推导{1，2，3}的类型
```

## 请记住

* auto 类型推导常常和 template 类型推导一样，但是 auto 类型推导假设初始化列表为 std::initializer_list，但是 template 类型推导不这么做
* auto 作为函数返回类型或 lambdas 表达式的形参类型时，默认使用 template 类型推导规则，而不是 auto 类型推导规则。

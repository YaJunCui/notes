# 条款15：尽可能使用constexpr

如果说 C++11 中有什么新东西能拿“最佳困惑奖”的话，那肯定是 constexpr 了。当把它用在对象上时，它本质上是 const 的加强版，但是把它用在函数上时，它将拥有不同的意义。切开“迷雾”（解开困惑）是值得的，因为当 constexpr 符合你想表达的情况时，你肯定会想要使用它的。

从概念上来说，constexpr 表明的一个值不只是不变的，它还能在编译期被知道。但是这个概念只是故事的一部分，因为当 constexpr 应用在函数上时，事情变得比看上去还要微妙。为了避免毁掉后面的惊喜，现在，我只能说你不能假设 constexpr 函数的返回值是 const 的，同时你也不能假设这些值能在编译期被知道。也许最有趣的是，这些东西都是特性（是有用的）。对于 constexpr 函数来说，不需要产生 const 或能在编译期知道的返回结果是一件好事。

但是，让我们从 constexpr 对象开始。这些对象确实是常量，也确实能在编译期被知道。（技术上来讲，它们的值是在翻译阶段被决定的，翻译阶段包含了编译期和链接期。除非你要写一个 C++ 的编译器或连接器，不然这都影响不到你，所以你能在编程的时候，开心地假设为 constexpr 对象的值是在编译期被决定的）

值能在编译期知道是很有用的。它们能代替只读内存，举个例子，尤其是对一些嵌入式系统来说，这是一个相当重要的特性。更广泛的应用就是，当 C++ 要求一个不变的，并且在编译期能知道的整形常量表达式的时候，我们可以使用它（constexpr 对象或函数）来替代。这样的场景包括了数组大小的说明，整形模板参数（包括std::array对象的长度），枚举成员的值，alignment说明符，等等。如果你想使用变量来做这些事，你肯定想要把它声明为 constexpr，因为编译器会确保它是一个编译期的值：

```C++
int sz;                             //non-constexpr变量
...

constexpr auto arraySize1 = sz;     //错误！sz的值不是在编译期被知道的
std::array<int, sz> data1;          //错误！同样的问题

constexpr auto arraySize2 = 10;     //对的，10是一个编译期的常量
std::array<int, arraySize2> data2;  //对的，arraySize2是一个constexpr
```

记住，const不能提供和constexpr一样的保证，因为const对象不需要用“在编译期就知道的”值初始化：

```C++
int sz;
...

const auto arraySize = sz;          //对的，arraySize是拷贝自sz的const变量

std::array<int, arraySize> data;    //错误！arraySize的值不能在编译期知道
```

简单来说，所有的 constexpr 对象都是 const 对象，但是不是所有的 const 对象都是 constexpr 对象。如果你想让编译器保证变量拥有的值能被用在那些，需要编译期常量的上下文中，那么你就应该使用 constexpr 而不是 const。

当涉及 constexpr 函数时，constexpr 对象的使用范围变得更加有趣。当使用编译期常量来调用这样的函数时，它们产生编译期常量。当用来调用函数的值不能在运行期前得知时，它们产生运行期的值。这听起来好像你知道它们会做什么，但是这么想是错误的。正确的观点是这样的：

* constexpr 函数能被用在要求编译期常量的上下文中。如果所有传入 constexpr 函数的参数都能在编译期知道，那么结果将在编译期计算出来。如果有任何一个参数的值不能在编译期知道，你的代码就被拒绝（不能在编译期执行）了。
* 当使用一个或多个不能在编译期知道的值来调用一个 constexpr 函数时，它表现得就像一个正常的函数，在运行期计算它的值。这意味着你不需要两个函数来表示相同的操作，一个为编译期常量服务，一个为所有的值服务。constexpr 函数把这些事都做了。

假设你需要一个数据结构来存放一个运算方式不会改变的实验结果。举个例子，在实验过程中，灯的亮度等级（有高，低，关三种状态），风扇的速度，以及温度也是这样，等等。如果这里有 n 种环境条件和实验有关，每种条件有三种状态，那么结果的组合数量就是 3^n。因此对于实验结果的所有的组合进行保存，就需要一个起码有 3^n 的空间的数据结构。假设每个结果都是一个 int，那么 n 就是在编译期已知的（或者说可以计算出来），std::array 是一个合理的选择。但是我们需要一个方法来在编译期计算 3^n。C++ 标准库提供了 std::pow，这个函数是我们需要的数学函数，但是对于我们的目的来说，它有两点问题。第一，std::pow 在浮点类型下工作，但是我们需要一个整形的结果。第二，std::pow 不是 constexpr（也就是，用编译期的值调用它时，它不能返回一个编译期的结果），所以我们不能用它来明确 std::array 的大小。

幸运的是，我们能自己写一个我们所需要的 pow，我马上就会告诉你怎么实现，但是现在先让我们看一下它是怎么声明以及怎么使用的：

```C++
constexpr                                   //pow是一个constexpr函数
int pow(int base, int exp) noexcept         //永远不会抛出异常
{
    ...                                     //实现在下面
}

constexpr auto numConds = 5;                //条件的数量

std::array<int, pow(3, numConds)> results;  //结果有3^numConds个函数
```

回忆一下，pow 前面的 constexpr 不是说 pow 返回一个 const 值，它意味着如果 base 和 exp 是编译期常量，pow 的返回结果能被视为编译期常量。如果 base 或 exp 不是编译期常量，pow 的结果将在运行期计算。这意味着 pow 不只能在编译阶段计算 std::array 的大小，它也可以在运行期的时候这么调用:

```C++
auto base = readFromDB("base");     //运行期得到这些值
auto exp = readFromDB("exponent");  

auto baseToExp = pow(base, exp);    //在运行期调用pow
```

因为当用编译期的值调用 constexpr 函数时，必须能返回一个编译期的结果，所以有些限制被强加在 constexpr 函数的实现上。C++11 和 C++14 有不同的限制。

在 C++11中，constexpr 函数只能包含一条简单的语句：一个 return 语句。实际上，限制没听起来这么大，因为两个技巧可以用来扩张 constexpr 函数的表达式，并且这将超过你的想象。第一，条件表达式 “?:” 能用来替换 if-else 语句，然后第二，递归能用来替换循环。因此 pow 被实现成这样：

```C++
constexpr int pow(int base, int exp) noexcept
{
    return (exp == 0 ? 1 : base * pow(base, exp - 1));
}
```

这确实可以工作，但是很难想象，除了写函数的人，还有谁会觉得这个函数写得很优雅。在 C++14 中，constexpr 函数的限制大幅度变小了，所以这让下面的实现成为了可能：

```C++
constexpr int pow(int base, int exp) noexcept           //C++14
{
    auto result = 1;
    for(int i = 0; i < exp; ++i) result *= base;

    return result;
}
```

constexpr 函数由于限制，只能接受和返回 literal 类型，本质上来说就是，这个类型的值能在编译期决定。在 C++11 中，除了 void 的所有 built-in 类型都是 literal 类型，user-defined 类型也可能是 literal 类型。因为构造函数和其他函数也可能是 constexpr：

```C++
class Point{
public:
    constexpr Point(double xVal = 0, double yVal = 0) noexcept
        : x(xVal), y(yVal)
    {}

    constexpr double xValue() const noexcept { return x;}
    constexpr double yValue() const noexcept { return y;}

    void setX(double newX) noexcept { x = newX; }
    void setY(double newY) noexcept { y = newY; }

private:
    double x, y;
};
```

这里，Point 的构造函数被声明为 constexpr，因为如果传入的参数能在编译期知道，则被构造的 Point 的成员变量的值也能在编译期知道。因此，Point 也能被初始化为 constexpr：

```C++
constexpr Point p1(9.4, 27.7);          //对的，在编译期“执行”constexpr构造函数  

constexpr Point p2(28.8, 5.3);          //也是对的
```

同样地，xValue 和 yValue 的获得者也能是 constexpr，因为如果用一个在编译期就知道的 Point 对象调用它们（比如，一个constexpr Point对象），则成员变量 x 和 y 的值都能在编译期知道。写一个获取 Point 对象的 constexpr 函数，然后用这个函数的返回值来初始化一个 constexpr 对象，这将成为可能。

```C++
constexpr
Point midpoint(const Point& p1, const Point& p2) noexcept
{
    return { (p1.xValue() + p2.xValue()) / 2,   //调用constexpr成员函数
             (p1.yValue() + p2.yValue()) / 2};  //并通过初始化列表产生一个新的临时Point对象
}

constexpr auto mid = midpoint(p1, p2);          //用constexpr函数的返回值来初始化一个constexpr对象
```

这是很激动人心的，它意味着，虽然 mid 对象的初始化需要调用构造函数，getter 函数和一个 non-member 函数，但是它还是能在 read-only 内存中创建！这意味着，你能使用一个表达式（比如 mid.xValue() * 10）来明确模板的参数，或者明确 enum 成员的值。它意味着以前运行期能做的工作和编译期能做的工作之间的界限变得模糊了，一些以前只能在运行期执行的运算现在可以移到编译期来执行了。移动的代码越多，软件跑得越快。（当然编译时间也会增加。）

在 C++11 中，有两个限制阻止 Point 的成员函数 setX 和 setY 被声明为 constexpr。第一，它们改动了它们操作的对象，但是在 C++11 中，constexpr 成员函数被隐式声明为 const。第二，它们的返回值类型是 void，void 类型在 C++11 中不是 literal 类型。在 C++14 中，两个限制都被移除了，所以 C++14 的 Point，能把它的 setter 也声明为 constexpr：

```C++
class Point{
public:
    ...
    constexpr void setX(double newX) noexcept       //C++14
    { x = newX; }
    constexpr void setY(double newY) noexcept
    { y = newY;}
    ...
};
```

这使得我们能写出这样的函数：

```C++
constexpr Point reflection(const Point& p) noexcept
{
    Point result;                       //创建一个non-constPoint

    result.setX(-p.xValue());           //设置它的x和y
    result.setY(-p.yValue());

    return result;                      //返回一个result的拷贝
}
```

客户代码看起来像这样：

```C++
constexpr Point p1(9.4, 27.7);
constexpr Point p2(28.8, 5.3);
constexpr auto mid = midpoint(p1, p2);

constexpr auto reflectedMid =           //reflectedMid的值是（-19.1 -16.5）
    reflection(mid);                    //并且是在编译期知道的
```

本条款的建议是，只要有可能就使用 constexpr，并且现在我希望你能知道这是为什么：比起 non-constexpr 对象和 non-constexpr 函数，constexpr 对象和 constexpr 函数都能被用在更广泛的上下文中（一些只能使用常量表达式的地方）。通过“只要有可能就使用 constexpr”，你能让你的对象和函数的使用范围最大化。

记住，constexpr 是对象接口或函数接口的一部分，constexpr 宣称“我能被用在任何需要常量表达式的地方”。如果你声明一个对象或函数为 constexpr，客户就有可能使用在这些上下文中（要求常量表达式的地方）。如果你之后觉得对于 constexpr 的使用是错误的，然后移除了 constexpr，这会造成很大范围的客户代码无法编译。（由于调试的原因，增加一个 I/O 操作到我们的 constexpr 函数中也会导致同样的问题，因为 I/O 语句一般不允许在 constexpr 中使用）“只要有可能就使用constexpr”中的“只要有可能”是说：需要你保证你愿意长时间保持这些对象和函数是 constexpr。

## 请记住

* constexpr 对象是 const，对它进行初始化的值需要在编译期知道。
* 如果使用在编译期就知道的参数来调用 constexpr 函数，它就能产生编译期的结果。
* 比起 non-constexpr 对象和函数，constexpr 对象和函数能被用在更广泛的上下文中。
* constexpr 是对象接口或函数接口的一部分。

# 条款 10：scoped enums 替代 unscoped enums

一般情况下，在花括号中声明一个 name（包括变量名，函数名），这个 name 的可见性会被限制在花括号的作用域内。对于在 C++98 风格的 enum 中声明的 enum 成员却不是这样。这些 enum 成员的 name 属于的作用域是 enum 所在作用域，这意味着在这个作用域中，不能拥有相同的 name:

```C++
enum Color { black, white, red };   //black，white，red和Color在同一个作用域
auto white = false;                 //错误！white在这个作用域已经声明过了
```

所以事实上，这些 enum 成员 name 泄露到 enum 所在的作用域中去了，这导致官方对于这种 enum 给出了一个官方术语：unscoped。新的 C++11 中有一个与此相对应的版本：scoped enum，不会像这样让 name 泄露：

```C++
enum class Color { black, white, red }; //black，white red在Color作用域中

auto white = false;                     //好的，没其他white

Color c = white;                        //错误！在这个作用域中没有一个叫“white”的enum成员
Color c = Color::white;                 //对的

auto c = Color::white;                  //也是对的（而且和 条款05 的建议一样）
```

因为 scoped enum 通过“enum class”声明，它们有时候也被叫做 enum 类。

单是减少命名空间的污染就足够作为理由让我们更偏爱 scoped enum 了，但是 scoped enum 还有第二个压倒性的优点：它们的成员属于强类型。unscoped enum 的成员能隐式转换到数值类型（然后，从数值类型，可以转换到浮点类型）。因此像下面这样，在语义上是扭曲的代码是完全有效的：

```C++
enum Color { black, white, red};        //unscoped enum

std::vector<std::size_t>                //函数，返回x的素因数
    primeFactors(std::size_t x);

Color c = red;
...

if(c < 14.5) {                  //把Color和double数比较（！）

    auto factors =              //计算Color的素因数（！）
        primeFactors(c);
    ...
}
```

然而，在“enum”后面添加一个简单的“class”，就能把 unscoped enum 转变为 scoped enum，并且情况会发生很大的改变。这里没有从 enum 的成员到任何其它类型的隐式转换：

```C++
enum class Color { black, white, red }; //scoped

Color c = Color::red;
...

if (c < 14.5) {                 //错误，不能把Color和double数进行比较

    auto factors =              //错误，函数需要一个std::size_t
        primeFactors(c);        //不能传一个Color进去
    ...
}
```

如果你真的想要把 Color 转换到不同的类型，你需要做的就是：使用 cast 把 Color 转换成你需要的类型：

```C++
if(static_cast<double>(c) < 14.5) { //奇怪的代码，但是有效

    auto factors =                  //可疑的，但是能通过编译
        primeFactors(static_cast<std::size_t>(c));
    ...
}
```

比起 unscoped enum，scoped enum 看起来还有第三个优点，因为 scoped enum 可以前置声明。也就是，他们的 name 可以在声明的时候不定义（不明确它们的成员）：

```C++
enum Color;         //错误

enum class Color;   //对的
```

这是一个误导。在 C++11 中，unscoped enum 也能前置声明，但是需要做一些额外的工作。这个工作源于一个事实，就是 C++ 中的每个 enum 都有一个整形的基础类型，这个类型由编译器决定。对于一个 unscoped enum，比如 Color：

```C++
enum Color { black, white, red };
```

编译器可能选择 char 来作为基础类型，因为这里只有三个值需要表示。然而，一些 enum 值的范围会大很多，比如：

```C++
enum Status { good = 0,
              failed = 1,
              incoplete = 100,
              corrupt = 200.
              indeterminate = 0xFFFFFFFF
            };
```

这里 enum 值需要表示的范围从 0 到 0xFFFFFFFF。除了一些不寻常的机器（在这些机器中，一个char至少由32bits组成），编译器就必须选择一个大于 char 的整形类型来表示 Status 的值。

为了内存的高效利用，只要一个基础类型能成功表示 enum 中的成员的值的范围，编译器常常会选择这个最小的基础类型。一些情况下，比起大小，编译器会优先考虑速度，在这种情况下，它们不一定会选择最小的基础类型，但是它们肯定也会在考虑速度的优化后，考虑大小的优化。为了实现这一点，C++98 只支持定义（所有的 enum 成员必须列出来）；enum的声明是不被允许的。这样，编译器才能在每个 enum 使用前，给每个 enum 选择一个基础类型。

但是“不能前置声明 enum”是有缺点的。最需要注意的是，它可能会增加编译依赖性。再次考虑 Status enum：

```C++
enum Status { good = 0,
              failed = 1,
              incomplete = 100,
              corrupt = 200,
              indeterminate = 0xFFFFFFFF
            };
```

这个 enum 可能需要在某个系统中使用，因此它被包含在头文件中，然后系统的每个部分都需要依赖它。如果一个新的 status 值被添加进来，

```C++
enum Status { good = 0,
              failed = 1,
              incomplete = 100,
              corrupt = 200,
              audited = 500,        //新值
              indeterminate = 0xFFFFFFFF
            };
```

这样很有可能整个系统都需要重新编译，甚至如果只是一个简单的子系统（更甚一个简单的函数）使用了这个 enum。这是被人们所讨厌的。这也是在 C++11 中，enum 前置声明消除的事（编译依赖性）。举个例子，这里有一个 scoped enum 的声明，它是完美有效的。并且一个函数用它作为一个参数：

```C++
enum class Status;                  //前置声明

void continueProcessing(Status s);  //使用声明过的enum
```

如果 Status 的定义修改了，并且头文件只包含这些声明就不需要重新编译。此外，如果 Status 修改了（比如，加了一个 audited 成员），但是 continueProcessing 的行为没受到影响（比如，因为 continueProcessing 没有使用audited），continueProcessing 的实现也不需要重新编译。

但是如果编译器需要在一个 enum 使用前知道它的大小，C++11 的 enum 怎么就能使用前置声明而 C++98 的 enum 却不行？回答很简单：scoped enum 的基础类型总是不变的，并且对于一个 unscoped enum，你也能明确它的基础类型。

通常情况下，一个 scoped enum 的基础类型是 int：

```C++
enum class Status;                  //基础类型是int
```

如果默认的情况不适合你，你可以自己设置：

```C++
enum class Status: std::uint32_t;   //Status的基础类型是std::uint32_t
```

不管怎么样，编译器都知道 scoped enum 的大小。

为了明确一个 unscoped enum 的基础类型，你需要做和 scoped enum 同样的事，这样就能做到前置声明了：

```C++
enum Color: std::uint8_t;           //unscoped enum的前置声明基础类型是std::uint8_t
```

基础类型也可以在enum定义的时候明确：

```C++
enum class Status: std::uint32_t { good = 0,
                                   failed = 1,
                                   incomplete = 100,
                                   corrupt = 200,
                                   audited = 500,
                                   indeterminate = 0xFFFFFFFF
                                 };
```

基于 scoped enum 能避免命名空间的污染，不会进行无意义的隐式类型转换的事实，这可能让你感到奇怪：这里起码有一种情况，unscoped enum 比起 scoped enum 会更有用一些。
比如我们在使用 C++11 的 std::tuple 的字段时。举个例子，假设对于一个社交网站的用户，我们想设计一个 tuple 持有 name，email 地址，reputation 值：

```C++
using UserInfo =
    std::tuple<std::string,     //name
               std::string,     //email
               std::size_t> ;   //reputation
```

只通过注释标明每个字段，那么在碰到分离的源文件时，注释将没有作用：

```C++
UserInfo uInfo;                 //tuple类型的对象
...
auto val = std::get<1>(uInfo);  //取字段1的值
```

作为一个程序员，你有很多方式来记录它。你真的能记住字段 1 代表的是用户的 email 地址吗？我想不是这样的。使用一个 unscoped enum 把名字关联到字段中去：

```C++
enum UserInfoFields { uiName, uiEmail, uiReputation };
UserInfo uInfo;
...
auto val = std::get<uiEmail>(uInfo);    //获得email字段的值
```

要让这起作用，必须要有从 UserInfoFields 到 std::size_t（std::get 需要的类型）的隐式转换。

用scoped enum实现的相应的代码会变得很繁琐：

```C++
enum class UserInfoFields { uiName, uiEmail, uiReputation };
UserInfo uInfo;
...
auto val = std::get<static_cast<std::size_t>(UserInfoFields::uiEmail)>(uinfo);
```

繁琐的情况可以通过写一个函数（传入一个 enum 成员，返回相应的 std::size\_t 值）来减少，但是这里有点微妙。std::get 是一个 template，并且你提供的值是 template 参数（注意使用尖括号，不是圆括号），所以转换函数（enum 成员到 std::size_t）必须在编译期产生结果。就像 条款15 解释的，这意味着它必须是 constexpr 函数。

事实上，它应该是 constexpr 函数 template，因为它应该能在任何 enum 下工作。并且如果我们让它继续泛化，我们应该把返回类型也泛化掉。比起返回一个 std::size\_t，我们需要返回 enum 的基础类型。通过 std::underlying\_type 类型萃取能做到这一点（见 条款09 中的 type traits 信息）。最后，我们声明它为 noexcept（见 条款14），因为我们知道它不会产生任何异常。最后会产生一个函数模板 toUType，它需要一个任意类型的enum成员，并且返回它的编译期 const 值（类型是基础类型）:

```C++
template<typename E>
constexpr typename std::underlying_type<E>::type
  toUType(E enumerator) noexcept
{
    return
      static_cast<typename std::underlying_type<E>::type>(enumerator);
}
```

在 C++14 中，通过用强大的 std::underlying\_type\_t(见 条款09)来替换 typename std::underlying\_type::type,toUType能被简化:

```C++
template<typename E>                    // C++14
constexpr std::underlying_type_t<E>
  toUType(E enumerator) noexcept
{
    return static_cast<std::underlying_type_t<E>>(enumerator);
}
```

在 C++14 中，更强大的 auto 返回类型（见 条款03）也同样有效：

```C++
template<typename E>                    // C++14
constexpr auto
  toUType(E enumerator) noexcept
{
    return static_cast<std::underlying_type_t<E>>(enumerator);
}
```

不管函数怎么写，toUType 允许我们这样访问 tuple 字段：

```C++
auto val = std::get<toUType(UserInfoFields::uiEmail)>(uInfo);
```

比起使用 unscoped enum，这里还是要写很多东西，但是它也避免了命名空间的污染，和你没有意识到的转换。在很多情况下，比起它的陷阱，你可能觉得多写几个字是合理（但是追溯到很久以前我们的数字通信还在用2400波特的调制解调器时，情况会不一样）。

## 你要记住的事

* C++98 风格的 enum 被称为 unscoped enum
* scoped enum 的成员只在 enum 中可见，它们只有在使用 cast 时才能转换到其它类型。
* scoped 和 unscoped enum 都支持自定义基础类型。scoped enum 的默认基础类型是int。unscoped enum没有默认基础类型。
* scoped enum 一直能前置声明，unscoped enum 只有在明确基础类型的情况下才能前置声明。

# 简单十步让你全面理解SQL

## 1 SQL 是声明性语言

首先需要思考的是，声明性。SQL 语言是为计算机声明了一个你想从原始数据中获得什么样的结果的一个范例，而不是告诉计算机如何能够得到结果。

> 简单地说，SQL 语言声明的是结果集的属性，计算机会根据 SQL 所声明的内容来从 `数据库` 中挑选出符合声明的数据，而不是像传统编程思维去指示计算机如何操作。

```sql
SELECT first_name, last_name FROM employees WHERE salary > 100000
```

这很容易理解，你无须关心员工的身份记录从哪来，你只需要知道谁有着不错的薪水。

* **我们从哪儿学习到这些？**
* 如果 SQL 语言这么简单，那么是什么让人们“闻 SQL 色变”？主要的原因是：我们潜意识中的是按照命令式编程的思维方式思考问题的。就好像这样：“电脑，先执行这一步，再执行那一步，但是在那之前先检查一下是否满足条件 A 和条件 B ”。例如，用变量传参、使用循环语句、迭代、调用函数等等，都是这种命令式编程的思维惯式。

## 2 SQL 的语法并不按照语法顺序执行

SQL 语句有一个让大部分人都感到困惑的特性，就是：SQL 语句的执行顺序跟其语句的语法顺序并不一致。SQL 语句的语法顺序是：

```sql
SELECT[DISTINCT]
FROM
WHERE
GROUP BY
HAVING
UNION
ORDER BY
```

为了方便理解，上面并没有把所有的 SQL 语法结构都列出来，但是已经足以说明 SQL 语句的语法顺序和其执行顺序完全不一样，就以上述语句为例，其执行顺序为：

```sql
FROM
WHERE
GROUP BY
HAVING
SELECT
DISTINCT
UNION
ORDER BY
```

关于 SQL 语句的执行顺序，有三个值得我们注意的地方：

* FROM 才是 SQL 语句执行的第一步，并非 SELECT 。数据库在执行 SQL 语句的第一步是将数据从硬盘加载到数据缓冲区中，以便对这些数据进行操作。
* SELECT 是在大部分语句执行了之后才执行的，严格的说是在 FROM 和 GROUP BY 之后执行的。理解这一点是非常重要的，这就是你不能在 WHERE 中使用在 SELECT 中设定别名的字段作为判断条件的原因。例如：

  ```sql
  SELECT A.x + A.y AS z
  FROM A
  WHERE z = 10 -- z 在此处不可用，因为SELECT是最后执行的语句
  ```
* 无论在语法上还是在执行顺序上， UNION 总是排在在 ORDER BY 之前。很多人认为每个 UNION 段都能使用 ORDER BY 排序，但是根据 SQL 语言标准和各个数据库 SQL 的执行差异来看，这并不是真的。尽管某些数据库允许 SQL 语句对子查询（subqueries）或者派生表（derived tables）进行排序，但是这并不说明这个排序在 UNION 操作过后仍保持排序后的顺序。

  > 注意：并非所有的数据库对 SQL 语句使用相同的解析方式。如 MySQL、PostgreSQL和 SQLite 中就不会按照上面第二点中所说的方式执行

* **我们学到了什么？**
* 既然并不是所有的数据库都按照上述方式执行 SQL 预计，那我们的收获是什么？我们的收获是永远要记得： SQL 语句的语法顺序和其执行顺序并不一致，这样我们就能避免一般性的错误。如果你能记住 SQL 语句语法顺序和执行顺序的差异，你就能很容易的理解一些很常见的 SQL 问题。

## 3 SQL 语言的核心是对表的引用（table references）

由于 SQL 语句语法顺序和执行顺序的不同，很多人会认为 SELECT 中的字段信息是 SQL 语句的核心。其实真正的核心在于对表的引用。

根据 SQL 标准，FROM 语句被定义为：

```SQL
<from clause> ::= FROM <table reference> [ { <comma> <table reference> }... ]
```

FROM 语句的“输出”是一张联合表，来自于所有引用的表在某一维度上的联合。我们们慢慢来分析：

```SQL
FROM a, b
```

上面这句 FROM 语句的输出是一张联合表，联合了表 a 和表 b 。如果 a 表有三个字段， b 表有 5 个字段，那么这个“输出表”就有 8（=5+3）个字段。

这个联合表里的数据是 a\*b，即 a 和 b 的笛卡尔积。换句话说，也就是 a 表中的每一条数据都要跟 b 表中的每一条数据配对。如果 a 表有3 条数据， b 表有 5 条数据，那么联合表就会有 15（=5\*3）条数据。

FROM 输出的结果被 WHERE 语句筛选后要经过 GROUP BY 语句处理，从而形成新的输出结果。我们后面还会再讨论这方面问题。

如果我们从集合论（关系代数）的角度来看，一张数据库的表就是一组数据元的关系，而每个 SQL 语句会改变一种或数种关系，从而产生出新的数据元的关系（即产生新的表）。

* **我们学到了什么？**
* 思考问题的时候从表的角度来思考问题提，这样很容易理解数据如何在 SQL 语句的“流水线”上进行了什么样的变动。

## 4 灵活引用表能使 SQL 语句变得更强大

灵活引用表能使 SQL 语句变得更强大。一个简单的例子就是 JOIN 的使用。严格的说 JOIN 语句并非是 SELECT 中的一部分，而是一种特殊的表引用语句。 SQL 语言标准中表的连接定义如下：

```sql
<table reference> ::=
    <table name>
  | <derived table>
  | <joined table>
```

就拿之前的例子来说：

```sql
FROM a, b
```

a 可能是如下表的连接：

```sql
a1 JOIN a2 ON a1.id = a2.id
```

将它放到之前的例子中就变成了：

```sql
FROM a1 JOIN a2 ON a1.id = a2.id, b
```

尽管将一个连接表用逗号跟另一张表联合在一起并不是常用作法，但是你的确可以这么做。结果就是，最终输出的表就有了 a1+a2+b 个字段了。

* **我们学到了什么？**

* 思考问题时，要从表引用的角度出发，这样就很容易理解数据是怎样被 SQL 语句处理的，并且能够帮助你理解那些复杂的表引用是做什么的。

  更重要的是，要理解 JOIN 是构建连接表的关键词，并不是 SELECT 语句的一部分。有一些数据库允许在 INSERT 、 UPDATE 、 DELETE 中使用 JOIN 。

## 5 SQL 语句中推荐使用表连接

我们先看看刚刚这句话：

```sql
FROM a,b
```

高级 SQL 程序员也许学会给你忠告：尽量不要使用逗号来代替 JOIN 进行表的连接，这样会提高你的 SQL 语句的可读性，并且可以避免一些错误。

利用逗号来简化 SQL 语句有时候会造成思维上的混乱，想一下下面的语句：

```sql
FROM a, b, c, d, e, f, g, h
WHERE a.a1 = b.bx
AND a.a2 = c.c1
AND d.d1 = b.bc
-- etc...
```

我们不难看出使用 JOIN 语句的好处在于：

* 安全。 JOIN 和要连接的表离得非常近，这样就能避免错误。
* 更多连接的方式，JOIN 语句能去区分出来外连接和内连接等。


* **我们学到了什么？**
* 记着要尽量使用 JOIN 进行表的连接，永远不要在 FROM 后面使用逗号连接表。

## 6 SQL 语句中不同的连接操作

SQL 语句中，表连接的方式从根本上分为五种：

```sql
EQUI JOIN
SEMI JOIN
ANTI JOIN
CROSS JOIN
DIVISION
```

### EQUI JOIN

这是一种最普通的 JOIN 操作，即等号连接，它包含两种连接方式：

```sql
INNER JOIN（或者是 JOIN ）   //只输出满足条件的的数据
OUTER JOIN（包括： LEFT 、 RIGHT、 FULL OUTER JOIN）
```

用例子最容易说明其中区别：

```sql
-- This table reference contains authors and their books.
-- There is one record for each book and its author.
-- authors without books are NOT included
author JOIN book ON author.id = book.author_id

-- This table reference contains authors and their books
-- There is one record for each book and its author.
-- ... OR there is an "empty" record for authors without books
-- ("empty" meaning that all book columns are NULL)
author LEFT OUTER JOIN book ON author.id = book.author_id
```

### SEMI JOIN

这种连接关系在 SQL 中有两种表现方式：使用 IN，或者使用 EXISTS。“ SEMI ”在拉丁文中是“半”的意思。这种连接方式是只连接目标表的一部分。这是什么意思呢？再想一下上面关于作者和书名的连接。我们想象一下这样的情况：我们不需要作者 / 书名这样的组合，只是需要那些在书名表中的书的作者信息。那我们就能这么写：

```sql
-- Using IN
FROM author
WHERE author.id IN (SELECT book.author_id FROM book)

-- Using EXISTS
FROM author
WHERE EXISTS (SELECT 1 FROM book WHERE book.author_id = author.id)
```

尽管没有严格的规定说明你何时应该使用 IN ，何时应该使用 EXISTS ，但是这些事情你还是应该知道的：
> 1 IN 比 EXISTS 的可读性更好  
> 2 EXISTS 比IN 的表达性更好（更适合复杂的语句）  
> 3 二者之间性能没有差异（但对于某些数据库来说性能差异会非常大）

因为使用 INNER JOIN 也能得到书名表中书所对应的作者信息，所以很多初学者机会认为可以通过 DISTINCT 进行去重，然后将 SEMI JOIN 语句写成这样：

```sql
-- Find only those authors who also have books  
SELECT DISTINCT first_name, last_name  
FROM author  
JOIN book ON author.id = book.author_id
```

这是一种很糟糕的写法，原因如下：
> SQL 语句性能低下：因为去重操作（ DISTINCT ）需要数据库重复从硬盘中读取数据到内存中。（译者注： DISTINCT 的确是一种很耗费资源的操作，但是每种数据库对于 DISTINCT 的操作方式可能不同）。
> 这么写并非完全正确：尽管也许现在这么写不会出现问题，但是随着 SQL 语句变得越来越复杂，你想要去重得到正确的结果就变得十分困难。

### ANTI JOIN

这种连接的关系跟 SEMI JOIN 刚好相反。在 IN 或者 EXISTS 前加一个 NOT 关键字就能使用这种连接。举个例子来说，我们列出书名表里没有书的作者：

```sql
-- Using IN  
FROM author  
WHERE author.id NOT IN (SELECT book.author_id FROM book)  

-- Using EXISTS  
FROM author  
WHERE NOT EXISTS (SELECT 1 FROM book WHERE book.author_id = author.id)  
```

关于性能、可读性、表达性等特性也完全可以参考 SEMI JOIN。
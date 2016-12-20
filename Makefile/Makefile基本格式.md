# Makefile基本格式

```C++
#目标：依赖（条件）
#  命令（tab，不是空格）
```

## 阶段1

```C++
app:
  gcc add.c sub.c dive.c mul.c main.c -o app
```

## 阶段2

```C++
app:add.o sub.o dive.o mul.o main.o
  gcc add.o sub.o dive.o mul.o main.o -o app

add.o:add.c
  gcc -c add.c
sub.o:sub.c
  gcc -c sub.c
dive.o:dive.c
  gcc -c dive.c
mul.o:mul.c
  gcc -c mul.c
main.o:main.c
  gcc -c main.c
```

## 阶段3

```C++
# $@表示目标，$^表示所有依赖文件，$<表示第一个依赖文件
obj=add.o sub.o dive.o mul.o main.o
src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))
target = app

$(target):$(obj)
  gcc $^ -o $@

%.o:%.c
  gcc -c $< -o $@
```

## 阶段4

```C++
CPPFLAGS= -Iinclude  #预处理器标志
CFLAGS= -g -Wall     #C编译器编译参数
LDFALGS=             #连接器链接选项，例如共享库等
CC=gcc               #编译器

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
TARGET = app

$(TARGET):$(OBJ)
  $(CC) $^ $(LDFLAGS) -o $@

%.o:%.c
  $(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

.PHONY:clean
clean:
  rm -f *.o
  rm -f app

test:
  @echo $(src)
  @echo $(obj)
```

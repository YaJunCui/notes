# python笔记

## 1 小知识的总结

1.1 在Python中，只有模块、类以及函数才会引入新的作用域，其它的代码块是不会引入新的作用域的。

## logging模块

### 基本用法

代码展示logging基本用法：

```python
# -*- coding:utf-8 -*-

import logging
import sys

# 获取logger实例，如果参数为空则返回root logger
logger = logging.getLogger("AppName")

# 指定logger输出格式
formatter = logging.Formatter("%(asctime)s %(levelname)-8s: %(message)s")

# 文件日志
file_handler = logging.FileHandler("test.log")
file_handler.setFormatter(formatter)      # 可以通过setFormatter指定输出格式

# 控制台日志
console_handler = logging.StreamHandler(sys.stdout)
console_handler.formatter = formatter     # 也可以直接给formatter赋值

# 为logger添加的日志处理器
logger.addHandler(file_handler)
logger.addHandler(console_handler)

# 指定日志的最低输出级别，默认为WARN级别
logger.setLevel(logging.DEBUG)

logger.debug("this is debug info")
logger.info("this is information")
logger.warn("this is warning message")
logger.error('this is error message')
logger.fatal('this is fatal message, it is same as logger.critical')
logger.critical('this is critical message')

# 移除一些日志处理器
logger.removeHandler(file_handler)
```

### Formatter日志格式
Formatter对象定义了log信息的结构和内容，构造时需要带两个参数：

* 一个是格式化的模板fmt，默认会包含最基本的level和 message信息
* 一个是格式化的时间样式datefmt，默认为 2003-07-08 16:49:45,896 (%Y-%m-%d %H:%M:%S)

fmt中允许使用的变量可以参考下表。

>%(name)s Logger的名字  
>%(levelno)s 数字形式的日志级别  
>%(levelname)s 文本形式的日志级别  
>%(pathname)s 调用日志输出函数的模块的完整路径名，可能没有  
>%(filename)s 调用日志输出函数的模块的文件名  
>%(module)s 调用日志输出函数的模块名  
>%(funcName)s 调用日志输出函数的函数名  
>%(lineno)d 调用日志输出函数的语句所在的代码行  
>%(created)f 当前时间，用UNIX标准的表示时间的浮点数表示   
>%(relativeCreated)d 输出日志信息时的，自Logger创建以来的毫秒数  
>%(asctime)s 字符串形式的当前时间。默认格式是“2003-07-08 16:49:45,896”。逗号后面的是毫秒  
>%(thread)d 线程ID。可能没有  
>%(threadName)s 线程名。可能没有  
>%(process)d 进程ID。可能没有  
>%(message)s 用户输出的消息   

### Configuration 配置方法

logging的配置大致有下面几种方式。

* 通过代码进行完整配置，主要是通过getLogger方法实现；
* 通过代码进行简单配置，主要是通过basicConfig方法实现；
* 通过配置文件，主要是通过 logging.config.fileConfig(filepath)

#### logging.basicConfig

basicConfig()提供了非常便捷的方式让你配置logging模块并马上开始使用，可以参考下面的例子。

```python
# -*- coding:utf-8 -*-

import logging

logging.basicConfig(filename='example.log',level=logging.DEBUG)
logging.debug('This message should go to the log file')

logging.basicConfig(format='%(levelname)s:%(message)s', level=logging.DEBUG)
logging.debug('This message should appear on the console')

logging.basicConfig(format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
logging.warning('is when this event was logged.')
```

#### 通过配置文件logging

如果你希望通过配置文件来管理logging，可以参考这个官方文档。

```python
# logging.conf
[loggers]
keys=root

[logger_root]
level=DEBUG
handlers=consoleHandler
#,timedRotateFileHandler,errorTimedRotateFileHandler

#################################################
[handlers]
keys=consoleHandler,timedRotateFileHandler,errorTimedRotateFileHandler

[handler_consoleHandler]
class=StreamHandler
level=DEBUG
formatter=simpleFormatter
args=(sys.stdout,)

[handler_timedRotateFileHandler]
class=handlers.TimedRotatingFileHandler
level=DEBUG
formatter=simpleFormatter
args=('debug.log', 'H')

[handler_errorTimedRotateFileHandler]
class=handlers.TimedRotatingFileHandler
level=WARN
formatter=simpleFormatter
args=('error.log', 'H')

#################################################
[formatters]
keys=simpleFormatter, multiLineFormatter

[formatter_simpleFormatter]
format= %(levelname)s %(threadName)s %(asctime)s:   %(message)s
datefmt=%H:%M:%S

[formatter_multiLineFormatter]
format= ------------------------- %(levelname)s -------------------------
 Time:      %(asctime)s
 Thread:    %(threadName)s
 File:      %(filename)s(line %(lineno)d)
 Message:
 %(message)s

datefmt=%Y-%m-%d %H:%M:%S
```

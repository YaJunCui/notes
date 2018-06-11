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



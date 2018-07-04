# WGSI协议

WSGI有两方：服务器方 和 应用程序。

* 服务器方：其调用应用程序，给应用程序提供环境信息和回调函数，这个回调函数是用来将应用程序设置的 http header 和 status 等信息传递给服务器方。

* 应用程序：用来生成返回的 header、body和status，以便返回给服务器方。

用 python 语言写的一个符合 WSGI 的 “Hello World” 应用程序如下所示：

```python

def app(environ, start_response):   
    start_response('200 OK', [('Content-Type', 'text/plain')])   
    yield "Hello world!\n"

```

其中

* 第一行定义了一个名为 app 的应用程序，接受两个参数，environ 和 start\_response，environ 是一个字典包含了 CGI 中的环境变量，start\_response也是一个callable，接受两个必须的参数，status（HTTP状态）和response\_headers（响应消息的头）;
* 第二行调用了start\_response，状态指定为“200 OK”，消息头指定为内容类型是 “text/plain”;
* 第三行将响应消息的消息体返回。

那通俗点来将的话：  

> wsgi 中的服务器方：我们可以理解成是 webserver，当然这个 webserver 可以是外置的，比如lighttpd，也可以是 python 自己写的。
> 而应用程序说白了就是：请求的统一入口！所有的请求都进入到这个 app 中来处理！ 这个 app 说白了就是一个函数！！（类中的__call__是一样的道理）

------------------------------------------------

这里有很多问题貌似没解决：

1. wsgi 一端的 webserver 是什么，这个貌似是很弱智的问题。比如 lighttpd 就是一个 webserver， 那我们在 lighttpd 配置文件中可以指定程序的入口脚本(比如 index.php)。那么所有请求都会转到这个 index.php 来执行。  
2. 那 gunicorn 呢？其虽然全部都是用 python 写的，但是原理和 lighttpd 一样的，内部都是开 socket 来监听请求，而后将请求转到后端的 python 脚本中去执行！当然了：其是转到一个函数中去执行而已！所以我们要做的就是实现这个函数，而后将之加载到 gunicorn 中，而后启动 gunicorn 即可！当然，这里的方式跟 lighttpd 有所区别，那边只要改一下配置文件，指定请求处理的入口脚本，然后重启 lighttpd 即可。而对于 gunicorn 而言，则是实现一个函数，而后将这个函数加载到 gunicorn 中，再运行一下 gunicorn！

前者 lighttpd 的很容易理解，后边的不太好理解，这里尝试着去理解吧。

```python

class ShireApplication(Application):
    # 初始化时调用
    def init(self, parser, opts, args):
        self.app_uri = args[0]
        sys.path.insert(0, os.getcwd())
 
    # 运行时调用
    def load(self):
        try:
            return util.import_app(self.app_uri)
        except Exception, e:
            exc = sys.exc_info()
            return make_tb_echo_app(exc)
 
def main():
    sys.argv += ['-c', os.path.join(os.path.dirname(__file__), 'gunicorn_config.py'), 'douban_wsgi',]
    ShireApplication("%prog [OPTIONS] APP_MODULE").run()

```

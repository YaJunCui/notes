# WGSI协议

WSGI有两方：服务器方 和 应用程序。

* 服务器方：其调用应用程序，给应用程序提供环境信息和回调函数，这个回调函数是用来将应用程序设置的http header和status等信息传递给服务器方。

* 应用程序：用来生成返回的header、body和status，以便返回给服务器方。

用 python 语言写的一个符合WSGI的 “Hello World” 应用程序如下所示：

```python

def app(environ, start_response):   
    start_response('200 OK', [('Content-Type', 'text/plain')])   
    yield "Hello world!\n"

```

其中

* 第一行定义了一个名为 app 的应用程序，接受两个参数，environ 和 start\_response，environ 是一个字典包含了CGI中的环境变量，start\_response也是一个callable，接受两个必须的参数，status（HTTP状态）和response\_headers（响应消息的头）;
* 第二行调用了start\_response，状态指定为“200 OK”，消息头指定为内容类型是 “text/plain”;
* 第三行将响应消息的消息体返回。

# WGSI协议

WSGI有两方：服务器方 和 应用程序。

①服务器方：其调用应用程序，给应用程序提供环境信息和回调函数，这个回调函数是用来将应用程序设置的http header和status等信息传递给服务器方。

②应用程序：用来生成返回的header、body和status,以便返回给服务器方。

用Python语言写的一个符合WSGI的“Hello World”应用程序如下所示：

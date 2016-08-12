# lastrpc

一个轻量级PHP端RPC的C扩展，支持二进制与json数据传输

----------------------------------------------------------------------------------


lastrpc核心尽量少的实现代码, 支持二进制数据传输，保证远程调用程序更高性能




## 环境：

- PHP 5.24
- Curl
- Json
- Msgpack (可选)


## 编译安装：

$/phpbinpath/phpize
$./configure --with-php-config=/phpbinpath/php-config/ (默认json传输) or 开启 使用二进制传输数据:
$./configure --with-php-config=/phpbinpath/php-config/ --enable-msgpack (可在php.ini中修改数据传输格式[lastrpc.packager=json])

$make && make install

## msgpack安装：

https://github.com/msgpack/msgpack-php


## ini 设置

- lastrpc.packager      // 数据传输格式, 使用--enable-msgpack, 使用msgpack传输, 否则json, 可选值有： "msgpack" "json"
- lastrpc.cntimeout     // 连接服务器超时时间, 默认1000毫秒
- lastrpc.rwtimeout     // 请求超时时间, 默认3000毫秒


## 客户端

```php
<?php

$client = new \lastrpc\Client('http://host/server.php');

$user = $client->getUserById('1968');

var_dump($user);

?>
```


## 服务端

```php
<?php

class User
{
    
    public function getUserById($userid)
    {
        return $userid;
    }
    
}

$server = new \lastrpc\Server(new User());
$server->handle();

?>
```





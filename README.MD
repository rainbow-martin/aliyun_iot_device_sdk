第三方依赖：
=============
|库名|下载地址|
| ------ | ------ |
|mqtt库|     https://github.com/eclipse/paho.mqtt.c|
|jsoncpp库|  https://github.com/open-source-parsers/jsoncpp|

﻿目录结构：
=============
|目录结构| 注释|
| ------ | ------ |
|3rd/|第三方依赖性，jsoncpp和paho mqtt|
|baselib/|基础类库，用于log记录等常见工具类|
|builall.sh*|*nix 系统编译脚本|
|iot_device_async/|异步mqtt接入iot 设备|
|iot_device_sync/|同步mqtt接入iot 设备|
|pre_make.sh|嵌入式设备（如ARM）编译器选择，用于交叉编译|
|win32/|windows vs2013编译工程|

- - -

windows系统编译版本
-------------
WIN32
   编译环境： VS2013<br/>
   编译工程： win32\mytest.sln<br/>
   编译方法：打开工程，选择Release-Win32, 即可进行编译
   注意：需要配置$(OpenSSLDir) 作为环境变量

*nix系统编译版本
-------------
*nix<br/>
   ./buildall.sh 即可<br/>
   编译产物 在bin目录<br/>

交叉编译编译版本
-------------
参考pre_make.sh，目前是arms树莓派的交叉编译环境
变编译之前运行
source pre_make.sh

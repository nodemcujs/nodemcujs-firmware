# nodemcujs [![Build Status](https://travis-ci.com/nodemcujs/nodemcujs-firmware.svg?branch=master)](https://travis-ci.com/nodemcujs/nodemcujs-firmware)

### A real JavaScript based interactive firmware for ESP32.

nodemcujs 是一个在 ESP32 芯片上的 JavaScript 运行时。不同于 NodeMcu，这是在 ESP32 芯片上运行了一个真正的 JavaScript 虚拟机。在 ESP32 上编写 JavaScript 就和编写 NodeJS 程序一样。并且提供了一个 32MBit 的片上虚拟文件系统，你可以编写模块化的应用，然后使用 require() 导入模块。甚至直接将你的兼容 NodeJS 模块运行在 ESP32 上，而无需做任何改动。

# 文档 | Documentation

website: https://timor.tech 正在编写文档，under the development

github: https://github.com/nodemcujs/nodemcujs-doc

这是 nodemcujs 的网站，所有文档和最新信息将会发布在这里。也可以通过 fork 项目贡献文章。文档还在不断完善中。

# 示例 | Examples

**WIFI**

- [连接 WIFI: examples/wifi/connect_to_ap.js](examples/wifi/connect_to_ap.js)
- [扫描 WIFI 热点: examples/wifi/scan_ap.js](examples/wifi/scan_ap.js)

**SPI**

- [st7735 1.44寸TFT屏幕: examples/spi/st7735-spi.js](examples/spi/st7735-spi.js)
- [st7789 1.3寸IPS屏幕: examples/spi/st7789-spi.js](examples/spi/st7789-spi.js)

**Sigmadelta Modulation**

- [led呼吸灯: examples/sigmadelta/led.js](examples/sigmadelta/led.js)

**Remote Control**

- [WS2812B 全彩LED: examples/remote_control/ws2812b.js](examples/remote_control/ws2812b.js)

# 已经支持的功能/模块 | Supported

**驱动**

- [x] WIFI: 目前支持 STA 模式，支持 WIFI 事件，扫描热点。AP 模式的 API 正在开发中
- [x] GPIO: 目前支持基本的 mode、write、read
- [x] SPI Master mode: 目前仅支持 HSPI
- [x] Sigmadelta second-order Modulation
- [x] Remote Contrl: 目前仅支持发送数据，可用于红外、WS2812
- [x] NVS FLASH: 用于保存系统信息，比如WIFI设置
- [x] ESP ERROR: 用于查找系统层统一的报错信息，格式化成可阅读的字符串

**Node**

- [x] 定时器，目前支持 setTimeout、setInterval、delay（同步非阻塞延时）
- [x] CMD模块系统，支持文件系统模块、内置模块、native模块
- [x] native Addons（需源码编译到固件，未来我们会支持 静态库）
- [x] 串口错误日志输出 (方便调试代码报错)

# 特性 | Feature

- 纯 C 开发，代码结构遵循 NodeJS，上手简单
- 支持 同步并且非阻塞 编程，能编写准实时、硬件驱动等高实时应用程序。
- 遵循 CMD 模块规范
- 使用开源 JerryScript，内存开销小，开源社区支持
- 完整 ES5， 部分 ES6 语法支持
- 虚拟文件系统
- 串口 shell 命令行交互，方便调试
- 使用官方 ESP_IDF 工程，集成硬件驱动

# Todo

- [ ] 事件循环
- [ ] 桥接驱动 IIC SPI
- [ ] tGFX 图形库
- [ ] 文件模块
- [ ] 调试功能
- [ ] 完善文档
- [ ] 更多。。。

# hello world

```js
var foo = require('/foo.js')

console.log('hello nodemcujs')

process.delay(1000) // sync and non-block

setTimeout(function() {
  console.log('timeout')
}, 1000)

console.log('hello world')
```

# WIFI hello world

```js
var wifi = require('wifi')

wifi.init();

wifi.setMode(wifi.WIFI_MODE.STA);

wifi.setConfig(wifi.WIFI_MODE.STA, {
  ssid: 'SSID',
  password: 'PASS',
  auth: wifi.WIFI_AUTH_MODE.WIFI_AUTH_WPA2_PSK
});

wifi.start();
wifi.connect();  // connect to ap
```

# 快速开始

快速在本地环境构建出可执行的固件并烧写到 ESP32 芯片上。

## 1. 开发环境搭建

项目使用 CMake `cmake_minimum_required (VERSION 3.5)` 构建。

我在 MacOS 10.13、Ubuntu 18.04.2 LTS、Windows 10 中已验证构建通过，你可以选择适合自己的开发环境。

在 Windows 中环境设置比较麻烦，请仔细参照官方文档进行环境安装，我多数在 Ubuntu 下进行开发测试。

### 1.1 获取 ESP-IDF (V4.3.1)

参照官方文档进行安装。注意本项目使用的是 V4.3.1 版本，理论上 v4 全系版本都支持的。

ESP-IDF(V4.3.1): https://docs.espressif.com/projects/esp-idf/zh_CN/v4.3.1/esp32/get-started/index.html#esp-idf

你也可以在没有安装 git 的环境中下载源码包: https://dl.espressif.com/dl/esp-idf/releases/esp-idf-v4.3.1.zip

### 1.2 设置工具链

在设置工具链前，请按照对应的系统安装必须的软件包：

- Windows: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/windows-setup.html
- Linux: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/linux-setup.html
- MaxOS: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/macos-setup.html

然后按照官方文档进行编译工具链的安装：

https://docs.espressif.com/projects/esp-idf/zh_CN/v4.3.1/esp32/get-started/index.html#get-started-set-up-tools。

## 2. 获取 nodemcujs 源码

```bash
$ git clone --recursive git@github.com:nodemcujs/nodemcujs-firmware.git
```

项目已经将 JerryScript 作为子模块，存放在 `/deps/jerryscript` 目录下。`clone` 的时候会一并将所有子模块 clone 下来。

如果你忘记了在 `clone` 时候加 `--recursive` 选项，那么你可以通过下面的命令单独 clone `子模块`。

```bash
$ git submodule update --init
```

## 3. 编译固件

先进入项目根目录：

```bash
$ cd nodemcujs-firmware
```

创建 build 文件夹，为了编译后的临时文件不影响源码目录。

```bash
$ mkdir build
$ cd build 
```

使用 Cmake 构建

```bash
$ cmake ../
```

可选步骤。配置构建参数。大多数情况下使用默认参数就可以，这里一般只需要配置好串口和波特率。

```bash
$ make menuconfig
```

> 注意: 项目使用了自定义的分区表。详情可以查看分区表文件 [partitions.csv][partitions.csv]

最后进行编译固件。

```bash
$ make
```

## 4. 烧录固件

如果编译成功，会生成 4 个文件：

1. nodemcujs.bin (可执行 app)
2. bootloader/bootloader.bin (引导)
3. partition_table/partition_table.bin (分区表)
4. storage.bin (用户文件镜像)

使用下面的命令进行固件的烧录。

```bash
$ make flash
```

如果你看到控制台输出如下信息，并一直停留，那么你需要手动让 ESP32 芯片进入下载模式。

```bash
esptool.py --chip esp32 -b 460800 write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x1000 bootloader/bootloader.bin 0x8000 partition_table/partition-table.bin 0x10000 nodemcujs.bin
esptool.py v2.6
Found 3 serial ports
Serial port /dev/ttyUSB0
Connecting........___........___
```

等待烧录完成，重启 ESP32 就可以了。

> 注意：make flash 会自动烧录文件镜像，此文件镜像就是 spiffs 目录下的文件。

系统上电会默认启动用户文件系统中的 /index.js，所以你的应用入口可以写在这里。

此外你还可以使用 ESPlorer 连接上 ESP32，输入 JavaScript 和它进行交互了。

## 5. 手动烧录固件

对于没有或者不方便安装 ESP-IDF 工程的用户，可以使用烧录工具进行烧录已经构建好的固件。

我们推荐使用 [esptool.py][esptool] 工具进行烧录。可以从 [release][release-github] 页面下载已经构建好的固件。

Tips: ESP-IDF 内置了 `esptool.py` 工具，可以直接使用。路径在 `$IDF_PATH/components/esptool_py/esptool/esptool.py`

```bash
$ python esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x1000 bootloader.bin 0x8000 partition-table.bin 0x10000 nodemcujs.bin 0x00110000 storage.bin
```

这里有几点需要说明：

> -b 参数表示下载固件时使用的波特率，如果出现烧录失败等问题，请尝试降低波特率为 115200 或者 9600。这可能是劣质的串口芯片造成的。
>
> -p 参数表示 ESP32 芯片在你电脑上的串口设备，请替换为实际的值或者端口号。在 Windows 上的可能值为 COM3。
>
> 0x1000 和 0x8000，以及 0x10000 使用的是默认值。
>
> 第一次烧录需要这 3 个文件，以后烧录只需要一个 nodemcujs.bin 文件就行了。
>
> storage.bin 是可选的，系统启动只需要前面三个固件即可。

## 6. 制作文件镜像

制作文件镜像有多种方式，我们推荐使用 nodemcujs 里面集成的方式: 

- 将你的 .js 文件或者其它文件放到项目根目录下的 `spiffs` 文件夹内，该文件夹下面的所有文件会被打包成一个 `storage.bin` 镜像。
- 然后执行 `make flash` 即可制作镜像并且自动烧录。

**手动制作文件镜像**

nodemcujs 使用 [spiffs][spiffs] 作为默认文件系统，容量大约为 `2.7MB`，所以文件的总大小不能超出此范围。关于为什么容量只有 2.7MB，请参考 [partitions.csv][partitions.csv]。

我们建议将要烧录到 flash 存储的文件放到 `spiffs` 文件夹内，在我们的构建系统中，我们将会自动构建 flash 镜像并随固件一起烧录。文件系统是默认以 `/` 为根目录的。

我们使用 [mkspiffs][mkspiffs] 来制作镜像。这是 C++ 工程，首先你要编译它，得到可执行文件 `mkspiffs`。

```bash
$ mkspiffs -c spiffs -b 4096 -p 256 -s 0x2F0000 storage.bin
```

上面的命令会将 `spiffs` 文件夹内的全部文件打包成镜像，并且在当前目录生成 `storage.bin` 文件。

这里有几点需要注意：

> -s 0x2F0000 是 nodemcujs 所使用的大小，至少在目前你不能大于此值。除非你自己定义分区表。
>
> 编译 mkspiffs 时需要传递参数: `CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"` 否则会出现 nodemcujs 文件系统无法工作。

## 7. 烧录文件到 flash 芯片

**手动烧录文件镜像**

nodemcujs 会在启动时检查分区，如果无法挂载 `storage` 分区，则会`自动格式化 storage` 分区并挂载。

你可以将你的 JavaScript 应用或者任何文件烧录到 ESP32 上，nodemcujs 会在启动时自动加载 `/spiffs/index.js` 文件，所以这可能是自动启动应用的一个好主意。

```bash
$ python esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x110000 spiffs.bin
```

使用上面的命令将文件镜像烧录到 flash 中。

有几点需要注意：

> 一旦你烧录文件镜像，则原来的分区会被覆盖掉，请知道你自己在做什么。
>
> -z 0x10000 是目前 nodemcujs 默认分区表参数，至少在目前你不能小于此值，否则 app 程序可能会被覆盖。

## 8. 更新 jerryscript

jerryscript 作为一个子模块放置在 `/deps/jerryscript` 目录下，所以更新 jerryscript 很方便，进入 `/deps/jerryscript` 目录，使用 `git` 拉取最新 `commit` 就行了。

或者手动下载最新 jerryscript 文件替换掉。

## 9. native 模块

硬件驱动部分或者性能要求高的部分，我们必须使用 C\C++ 编写，这个时候就需要用到 `native` 模块了。比如 `GPIO` 模块。

在这之前，有几个关于模块的概念必须要搞清楚：

- 第三方模块：用户编写的，存在于文件系统上的 .js 文件，我们叫第三方模块。
- 内置js模块：编译到 nodemcujs 固件里面的 .js 文件，我们叫内置模块，比如 path 模块。它们存在 /main/src/js/ 文件夹下面。
- native模块：使用 C\C++ 编写的模块，我们叫 native模块。比如 GPIO。

> 注意：通常内置模块包含一个对应的 native模块，我们把 内置js 和 native模块 统称为 内置模块。比如 console。
> 如果一个内置模块有对应的 native模块，则 nodemcujs 会自动将对应的 native模块 注入到 内置js模块中，通过 native 全局变量引用。

`native` 模块是由 C\C++ 编写的模块，它和 JS 文件模块的使用方法是一样的。native模块以 `nodemcujs_module_xxx.c` 命名，存放在 `/main/src/modules` 文件夹下，我们规定 `native` 模块都必须有一个 `nodemcujs_module_init_xxx` 方法用于导出，该函数的返回值是 `jerry_value_t` 类型。

下面我们以编写 `GPIO` 模块为例。

首先我们编写该模块的 init 函数：

```c
// main/src/modules/nodemcujs_module_gpio.c

jerry_value_t nodemcujs_module_init_gpio()
{
    jerry_value_t gpio = jerry_create_object();
    // ......
    return gpio;
}
```

新增native模块后，需要重新执行 `cmake ..` 构建。然后我们就可以通过 `var gpio = require('gpio')` 使用了。

此外，我们还可以再编写一个对应的内置js模块，把native模块通过js包装成更友好的API：

```js
// main/src/js/gpio.js

var gpio = native; // 这个全局变量 native 就是 C函数 nodemcujs_module_init_gpio 返回的对象

function GPIO(pin) {
  this.pin = pin;
}

GPIO.prototype.mode = function(mode) {
  gpio.mode(this.pin, mode);
}

GPIO.prototype.write = function(level) {
  gpio.write(this.pin, level);
}

module.exports = GPIO;
```

最后在 JS 中使用它：

```js
var GPIO = require('gpio') // 此时 gpio 就是 gpio.js 到处的对象了
```

`native` 模块是有 `缓存` 的，init 方法被调用后，会将模块的值缓存起来，以后再次 `require` 将会直接返回缓存的值。

# FAQ | 常见错误一览

请前往官方网站查看。

# License

[MIT][MIT]




[esptool]: https://github.com/espressif/esptool
[release-github]: https://github.com/nodemcujs/nodemcujs-firmware/releases
[partitions.csv]: ./partitions.csv
[mkspiffs]: https://github.com/igrr/mkspiffs
[spiffs]: https://docs.espressif.com/projects/esp-idf/zh_CN/v4.3.1/esp32/api-reference/storage/spiffs.html
[MIT]: [./LICENSE]

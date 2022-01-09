# nodemcujs [![Build Status](https://travis-ci.com/nodemcujs/nodemcujs-firmware.svg?branch=master)](https://travis-ci.com/nodemcujs/nodemcujs-firmware)

> 目前处于开发阶段，还有很多需要做

### A real JavaScript based interactive firmware for ESP32.

nodemcujs 是一个在 ESP32 芯片上的 JavaScript 运行时。不同于 NodeMcu，这是在 ESP32 芯片上运行了一个真正的 JavaScript 虚拟机。在 ESP32 上编写 JavaScript 就和编写 NodeJS 程序一样。并且提供了一个 32MBit 的片上虚拟文件系统，你可以编写模块化的应用，然后使用 require() 导入模块。甚至直接将你的兼容 NodeJS 模块运行在 ESP32 上，而无需做任何改动。

# 文档 | Documentation

website: http://nodemcujs.timor.tech

github: https://github.com/nodemcujs/nodemcujs-doc

这是 nodemcujs 的网站，所有文档和最新信息将会发布在这里。也可以通过 fork 项目贡献文章。文档还在不断完善中。

# 特性

- 串口命令行交互
- 使用开源 JerryScript，内存开销小，开源社区支持
- 完整 ES5， 部分 ES6 语法支持
- 虚拟文件系统
- 遵循 CMD 模块规范
- 使用官方 ESP_IDF 工程，集成硬件驱动
- 纯 C 开发，编写 addon 方便

# Todo

- [x] require 相对路径支持
- [x] 内置模块
- [x] native 模块
- [x] 错误输出
- [ ] 调试功能
- [ ] 桥接驱动
- [ ] 完善文档
- [ ] 更多。。。

# hello world

```js
var foo = require('/foo.js')

setInterval(() => {
    console.log('hello nodemcujs!')
}, 1000)

foo();
```

# 快速开始

快速在本地环境构建出可执行的固件并烧写到 ESP32 芯片上。

## 1. 开发环境搭建

项目使用 CMake `cmake_minimum_required (VERSION 2.8.12)` 构建。

我在 MacOS 10.13、Ubuntu 18.04.2 LTS、Windows 10 中已验证构建通过，你可以选择适合自己的开发环境。

在 Windows 中环境设置比较麻烦，请仔细参照官方文档进行环境安装，我多数在 Ubuntu 下进行开发测试。

### 1.1 设置工具链

按照官方文档进行编译工具链的安装。注意 ESP-IDF 的版本和 xtensa 工具链的版本，请使用以下页面中的工具链版本。

- Windows: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/windows-setup.html
- Linux: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/linux-setup.html
- MaxOS: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/macos-setup.html

### 1.2 获取 ESP-IDF (V3.2-RC)

参照官方文档进行安装。注意本项目使用的是 V3.2-RC 版本。

ESP-IDF(V3.2-RC): https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/get-started/index.html#esp-idf

你也可以在没有安装 git 的环境中下载源码包: https://github.com/espressif/esp-idf/releases/download/v3.2/esp-idf-v3.2.zip

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

如果编译成功，会生成 3 个文件：

1. nodemcujs.bin (可执行 app)
2. bootloader/bootloader.bin (引导)
3. partition_table/partition_table.bin (分区表)

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

等待烧录完成，重启 ESP32 就可以了。你可以使用 ESPlorer 连接上 ESP32，输入 JavaScript 和它进行交互了。

## 5. 手动烧录固件

对于没有或者不方便安装 ESP-IDF 工程的用户，可以使用烧录工具进行烧录已经构建好的固件。

我们推荐使用 [esptool.py][esptool] 工具进行烧录。可以从 [release][release-github] 页面下载已经构建好的固件。

Tips: ESP-IDF 内置了 `esptool.py` 工具，可以直接使用。路径在 `$IDF_PATH/components/esptool_py/esptool/esptool.py`

```bash
$ python esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x1000 bootloader.bin 0x8000 partition-table.bin 0x10000 nodemcujs.bin
```

这里有几点需要说明：

> -b 参数表示下载固件时使用的波特率，如果出现烧录失败等问题，请尝试降低波特率为 115200 或者 9600。这可能是劣质的串口芯片造成的。
>
> -p 参数表示 ESP32 芯片在你电脑上的串口设备，请替换为实际的值或者端口号。在 Windows 上的可能值为 COM3。
>
> 0x1000 和 0x8000，以及 0x10000 使用的是默认值。
>
> 第一次烧录需要这 3 个文件，以后烧录只需要一个 nodemcujs.bin 文件就行了。

## 6. 制作文件镜像

nodemcujs 使用 [spiffs][spiffs] 作为默认文件系统，容量大约为 `2.7MB`，所以文件的总大小不能超出此范围。关于为什么容量只有 2.7MB，请参考 [partitions.csv][partitions.csv]。

我们建议将要烧录到 flash 存储的文件放到 `spiffs` 文件夹内，未来的构建系统中，我们将会自动构建 flash 镜像并随固件一起烧录。文件系统也是默认以 `/spiffs` 为前缀的。

我们使用 [mkspiffs][mkspiffs] 来制作镜像。这是 C++ 工程，首先你要编译它，得到可执行文件 `mkspiffs`。

```bash
$ mkspiffs -c spiffs -b 4096 -p 256 -s 0x2F0000 spiffs.bin
```

上面的命令会将 `spiffs` 文件夹内的全部文件打包成镜像，并且在当前目录生成 `spiffs.bin` 文件。

这里有几点需要注意：

> -s 0x2F0000 是 nodemcujs 所使用的大小，至少在目前你不能大于此值。除非你自己定义分区表。
>
> 编译 mkspiffs 时需要传递参数: `CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"` 否则会出现 nodemcujs 文件系统无法工作。

## 7. 烧录文件到 flash 芯片

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

`native` 模块是由 C\C++ 编写的模块，它和 JS 文件模块的使用方法是一样的，都使用 `require` 引入。一个 `native` 模块就是一个 `component`，以 `nodemcujs_module_xxx` 命名。我们规定 `native` 模块都必须有一个 `nodemcujs_init_xxx` 方法用于导出，该函数的返回值是 `jerry_value_t` 类型。然后在 `components/nodemcujs_buildin/include/nodemcujs_module_inl.h` 中注册模块才能在 JS 中使用。

下面我们以注册 `GPIO` 模块为例。

首先我们编写该模块的 init 函数：

```c
// components/nodemcujs_module_gpio/nodemcujs_module_gpio.c

jerry_value_t nodemcujs_init_gpio()
{
    jerry_value_t gpio = jerry_create_object();
    // ......
    return gpio;
}
```

然后再注册该模块：

```c
// components/nodemcujs_buildin/include/nodemcujs_module_inl.h

extern jerry_value_t nodemcujs_init_gpio();

const nodemcujs_module_t nodemcujs_modules[] = {
  { "gpio", nodemcujs_init_gpio }
};
```

只需要 2 步我们就完成了 native 模块的注册，最后可以在 JS 中使用它了：

```js
var gpio = require('gpio')
```

`native` 模块是有`缓存`的，init 方法被调用后，会将模块的值缓存起来，以后再次 `require` 将会直接返回缓存的值。

# FAQ | 常见错误一览

请前往官方网站查看。

# License

[MIT][MIT]




[esptool]: https://github.com/espressif/esptool
[release-github]: https://github.com/nodemcujs/nodemcujs-firmware/releases
[partitions.csv]: ./partitions.csv
[mkspiffs]: https://github.com/igrr/mkspiffs
[spiffs]: https://docs.espressif.com/projects/esp-idf/zh_CN/v3.2-rc/api-reference/storage/spiffs.html
[MIT]: [./LICENSE]

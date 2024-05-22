# ESP32c3_FFT_LEDmatrix

## 条件

需要的元件：Arduino ESP32c3、MAX9814麦克风模块、MAX72198*8LED阵列模块

环境：Windows10家庭版、Arduino IDE v2.0.4

使用的库函数：LedControl v1.0.6、ArduinoFFT v1.6.1

## 原理图

![原理图](https://github.com/nightvoyageryhx/ESP32c3_FFT_LEDmatrix/blob/main/Schematic_fft_led_2024-05-22.png)

**********

**如果使用的是ESP32C3，在编译前请进行以下操作** 

将```C:文档\Arduino\libraries\LedControl\src``` 路径（也就是LEDControl库的安装路径）下的```LedControl.h```文件中的
```
#include <avr/pgmspace.h>
```

注释掉或者删除（大约在第30行），**否则会报错**

如果不知道路径在哪，把鼠标指针放到代码里```#include <LedControl.h>```这一行IDE就会显示出来

*****************

## 连线

参考原理图

| ESP32c3 | MAX9814 | MAX7219 |
| :-----: | :-----: | :-----: |
|  GPIO9  |\         | CLK |
| GPIO10  | \        | CS |
| GPIO12 | \ | DIN |
| GPIO1 | OUT | \ |
| 5V | VCC | VCC |
| GND | GND | GND |



> ESP32c3**不推荐使用GPIO11**，如果要使用需要请先进行解锁 

********

后续会在b站上传演示

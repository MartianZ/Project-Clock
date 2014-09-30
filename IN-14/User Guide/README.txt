辉光管时钟程序使用说明书

连接辉光管时钟设备到电脑：

1. 连接辉光管电源，短路电池右侧的时钟芯片的上方的TX和RX。
2. 连接辉光管USB数据线到操作系统
3. 若使用Windows操作系统，安装驱动文件stmcdc.inf；Linux最新Kernel和OS X 10.9及以上版本系统自带设备驱动。
4. Windows系统下安装驱动后，设备管理器 - 端口（COM和LPT）中会出现 Sekaisen Hendoritsu Meta 设备。Mac和Linux系统下会在/dev下新增tty设备，lsusb命令会提示新增IBM Cash Drawer设备。
5. Windows系统下强烈推荐采用Putty，Linux和OS X系统推荐采用minicom软件，连接辉光管的串口，波特率自定，推荐115200或更高，数据位8，停止位1，无校验和流控制。

配置时间：

1. 连接到系统的辉光管串口设备。
2. 在终端下按键盘?按键，辉光管自动发回指令帮助信息，如下所示

Sekaisen Hendoritsu Meta (Rev 1.1)
 p - Print date
 c - Configuration mode
 e - Enable/Disable NixieTube
 z - Enable/Disable automatically turn off NixieTube between 1:00 to 7:00 am
 l - Print 'ILWT' 100 times!

3. 根据指令，键盘按下小写p，辉光管返回当前时间，可利用该扩展开发其他外设，发送小写e，开启和关闭辉光管时钟显示，发送小写z，开启或关闭根据时间自动关闭辉光管的休眠功能。发送小写c，进入配置模式。
4. 配置模式说明如下

Configuration Mode:
 h - Set hour
 m - Set minute
 s - Set second
 y - Set year
 o - Set month
 d - Set day
 w - Set day of week
 r - Set the positive duty cycle of red leds
 g - Set the positive duty cycle of green leds
 b - Set the positive duty cycle of blue leds

5. 配置模式下，根据提示发送字母指令可以修改相关信息，例如按下h，系统返回：
Enter TWO digits to set a new HOUR value...
此时输入22，系统返回Value Set，同时辉光管的时间也自动更新并保存，并自动退出配置模式。

6. 若需要修改辉光管的自动开启和关闭时间，请修改nixietube.c源代码下的相关代码，重新编译后烧写。因为我实在是太懒了 (´・ω・｀)。
7. 配置过程中若程序出现故障，辉光管会自动复位，此时需要重新连接USB线缆重新运行串口程序即可继续配置。

按键说明：

PCB Rev.White版本共有4个按键。

S1按键为系统复位按键。
S2按键在正常工作时按下可关闭或开启辉光管显示，上电时按住S2按键可进入辉光管自检模式。注意当辉光管自动休眠后，按下S2按键会开启辉光管显示同时不会再次自动进入休眠状态，需要重返休眠状态按下S1按钮复位系统即可。
S3、S4按键在正常工作时按下可实现秒的微调，方便在时间略不准确时进行快捷调节。

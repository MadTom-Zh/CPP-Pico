# Raspberry Pi Pico使用的程序，基于C++；

# libraries&examples

更新时间 2024-08-13

自己写的类库，其他程序使用到的，如果有更新，会同时更新到这里（基本上）；

### SerialCom

更新时间 2024-08-13

用于实现通过USB串口来进行通讯的快速框架，使用时注册命令，在客户端执行命令后，服务端自动解析命令和参数，并执行对应的方法；

具体使用，请见下面调用了这个库的程序，更好理解；

### StepperMotorControl

更新时间 2024-08-13

自制的步进电机脉冲发生器；优势在于实现了变速和变动转动；

原理是通过设置基本参数（匀速和变速），预先计算启停序列，执行转动时复制序列并按照序列生成和输出脉冲；

变动转动则是执行命令后计算序列（因为每次最终速度不一定一样，所以每次生成的序列也不一样），如果内存充足，则计算后生成和输出脉冲；

按照pico引脚数量，默认提供同时控制8个步进电机，但我测试的时候只接了1个；另外因为pico内存很小，同时设置和控制多个电机的话，估计会很难办；

解释和演示视频：

**因为pico内存限制，当变速、变动区间过长时，会包内存不足，且内存剩余估算问题，偶尔会内存溢出，所以正式使用时，请做好充分的调试调优；

# 【具体项目】

### ComQATest

更新时间 2024-08-13

用于设计和编写SerialCom，实现电脑端通过USB和pico进行快速通信；

匹配的客户端SemiAutoSerialCom，请从CS.net库中获取；

演示视频： 【】

### StepperMotor57Test

更新时间 2024-08-13

用于设计和编写StepperMotorControl，调用了SerialCom；

演示视频： 【】
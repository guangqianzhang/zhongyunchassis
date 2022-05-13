## 说明
2022/4/25
张光钱

这是一个可用的第一版底层控制。可以正常控制
+ 完成速度闭环pid
+ 转角控制

+ **chassis2** 这是一个功能包模块，需要下载到工作空间的src目录下
+ 控制指令如**msg/cmd.msg**文件数据格式所示。
+ 每一帧控制报文都需要包含上述文件中的控制指令。
+ 控制时**control_enable**需要=1。前进、后退以及空挡需要改变对应**gear**，具体见上述**cmd.msg**文件。

## 使用前加载can模块
+` sudo modprobe gs_usb`
+ 或者尝试以下：
    + modprobe can      // 插入 can 总线子系统
    + modprobe can-raw   //插入can 协议模块
    + modprobe can-bcm
    + modprobe can-gw  
    + modprobe can_dev
    + modprobe mttcan  

## can配置
端口：can0
波特率：500KB 
restart：100ms
运行文件夹中`sudo ./canable.sh`

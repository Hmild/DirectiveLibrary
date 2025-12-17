# DirectiveLibrary
通用的指令收发库, 包括但不限于AT指令, 协议指令
代码整体4层架构, AT_Hardware_Port.h AT_Core.h AT_Network.h AT_Protocol
AT_Hardware_Port.h 硬件接口层, 需要用户使用头文件接口函数注册底层硬件驱动
AT_Core,h 核心层, 用户可以使用基于底层硬件驱动函数的功能函数, 以及配置buffer
AAT_Network.h 网络层, 用户通过接口函数添加, 删除, 设置, 网络指令, 内部自动发送, 解析网络指令
AAT_Protocol.h 协议层, 用户通过接口函数添加, 删除, 设置, 协议指令, 内部会自动解析(发送)用户需要的协议指令
批注: 详细内容查看文件内的数据手册

#ifndef __AT_PROTOCOL_H
#define __AT_PROTOCOL_H

/* 初次使用建议和AT库数据手册一同使用 */



/* 协议指令总数量 */
#define AT_PROTOCOL_MENU_SIZE		20

/* 连包处理最大次数 */
#define AT_PROTOCOL_LINK_BAG_MAX			3

/* 发送任务 队列大小 实测20够用 */
#define AT_PROTOCOL_SEND_TASK_QUEUE_MAX		20




/* 协议解析,发送任务,放入主循环扫描, AT_NETWORK_ON 执行 */
void AT_Protocol_Task(void);

/* 心跳,端口状态,心跳超时计数器递增,扫描任务,强烈建议 1ms 扫描1次, AT_NETWORK_ON 执行 */
void AT_Protocol_DeviceState_Task(void);


/* 协议菜单状态枚举 */
typedef enum
{
	AT_PROTOCOL_MENU_EXCEED = 0,
	AT_PROTOCOL_MENU_PARAM_DEFAULT,
	
	AT_PROTOCOL_MENU_SUCCEED,

}AT_Protocol_MenuState_t;


/**
  * @brief  添加协议指令函数

  * @note   使用方法：
							无需校验的数据: AT_Protocol_Directive_Add(TT_LOGIN, TT_Login_Build, TT_Login_Recv, NULL);
							需校验的数据: 	AT_Protocol_Directive_Add(TT_CTRL, TT_Ctrl_Build, TT_Ctrl_Recv, "\"msgType\":500");
	* @param  directive_index: 添加指令对应的索引, 范围 0 - AT_PROTOCOL_MENU_SIZE ;

	* @param  void(*build_func)(uint8_t port): 构建待发送协议内容,示例代码如下;
							注意: 无需指定端口的指令,就不需要使用 port ;
							void TT_Login_Build(uint8_t port)
							{
								...
								Json_End(&JsonStructure); // 数据包构建完成
								
								将数据包起始地址,长度传给发送任务状态机,构建的任务就完成了(该值不可以是局部的);
								AT_Protocol_Send_SM_DataPacket_Write(JsonStructure.buf, JsonStructure.index);
							}

	* @param  char *(*recv_func)(char *addr): 接收平台回复(下发)指令,示例代码如下;
							char *TT_Login_Recv(char *addr) { return NULL; }

							addr: DMA 环形缓冲区中第1次出现 directive 的地方;
							返回值: 接收成功,返回 !NULL ,失败或无,返回 NULL;

  * @param  directive: 需要识别的指令标识,例如: EBCharge 无需校验则填 NULL
  * @retval AT_Protocol_MenuState_t 类型的状态
  */
AT_Protocol_MenuState_t AT_Protocol_Directive_Add(uint32_t directive_index, void(*build_func)(uint8_t port), char *(*recv_func)(char *addr), char *directive);


/**
	* @brief  协议发送状态机数据包写入函数
  * @note   构建完数据包后,将数据包起始地址,长度传给发送状态机
  * @param  packet_addr: 数据包起始地址
  * @param  packet_len: 数据长度
  * @retval AT_Protocol_MenuState_t 类型的状态
  */
AT_Protocol_MenuState_t AT_Protocol_Send_SM_DataPacket_Write(char *packet_addr, uint32_t packet_len);


/**
	* @brief  协议发送任务状态机入队函数
	* @note   主循环扫描,当 network_flag == AT_NETWORK_ON 在入队,否则设备长时间离线,任务队列被旧任务填满,后续上线后会发送旧任务数据,
	* @param  directive_index: 待发送指令的索引,必填;
	* @param  port: 指定发送端口,无泽填0;
	* @retval 返回 0 入队失败, 返回 1 入队成功;
	*/
uint8_t AT_Protocol_SendQueue_Enqueue(uint32_t directive_index, uint8_t port);



/* 网络模块参数类型 */
typedef struct
{
	char *send_cmd;				//模块发送数据前的发布指令,						默认 "AT+QISEND=0,%d\r\n"
	char *recv_urc;				// 模块接收到服务器消息后上报的 URC, 	默认 "+QIURC: \"recv\""
	uint16_t send_max_len;// 模块单次发送的最大数据量, 					默认 1440

}AT_Protocol_ModuleParam_t;

/**
  * @brief  网络模块参数配置
  * @note   send_cmd, recv_urc, 不可传入 NULL(如有模块无发送指令跟接收URC,联系库开发者,对库进行迭代升级)
  * @param  AT_Protocol_ModuleParam_t 类型的指针
  * @retval AT_Protocol_MenuState_t 类型的状态
  */
AT_Protocol_MenuState_t AT_Protocol_ModuleParamConfig(AT_Protocol_ModuleParam_t *module_param_cfg);



/**
  * @brief  设置端口状态指令时间阈值(内部默认60000)
  * @note   当 AT_Protocol_DeviceState_Task() 扫描周期为 1 ms 时：端口状态发送周期 = value ms 
  * @param  value: 范围 0 - uint32_t 最大值
  * @retval 无
  */
void AT_Protocol_PortStateThreshold_Set(uint32_t value);

/**
  * @brief  设置心跳指令时间阈值(内部默认30000)
  * @note   当 AT_Protocol_DeviceState_Task() 扫描周期为 1 ms 时：心跳发送周期 = value ms 
  * @param  value: 范围 0 - uint32_t 最大值
  * @retval 无
  */
void AT_Protocol_BeatThreshold_Set(uint32_t value);

/**
  * @brief  设置心跳超时时间阈值(内部默认50000,建议根据心跳指令时间调整)
  * @note   当 AT_Protocol_DeviceState_Task() 扫描周期为 1 ms 时：心跳超时周期 = value ms 
  * @param  value: 范围 0 - uint32_t 最大值
  * @retval 无
  */
void AT_Protocol_BeatTimeoutThreshold_Set(uint32_t value);


/**
  * @brief  指令发送超时重试时间设置,举例,当发送 "AT+QISEND=0,%d\r\n" 后,模块超过这个 SumTime,
							会置位 AT_PROTOCOL_BEAT_TIMEOUT_FLAG 离线标志
  * @note   * Freq_Ref: 168MHZ(基准频率)
						* Once_Ref: 0.00001057s(基准频率下重试1次的时间)
						* 计算不同频率下单次重试时间公式: Once_Now  = Once_Ref * (Freq_Ref / Freq_Now)
					  * 总时间公式: SumTime = Once_Now * value,单位s
  * @param  value: 0 - uint32_t 最大值 默认 0x000EFFFF
  * @retval 
  */
void AT_Protocol_RetryThreshold_Set(uint32_t value);





/* 设备状态标志位获取 */
uint32_t AT_Protocol_DeviceStateFlag_Get(void);

/**
  * @brief  设备状态标志位设置
  * @note   
  * @param  device_state_flag:
							AT_PROTOCOL_LOGIN_SEND_FLAG,AT_PROTOCOL_PORT_STATE_SEND_FLAG,AT_PROTOCOL_BEAT_SEND_FLAG,
							AT_PROTOCOL_BEAT_TIMEOUT_FLAG;
  * @retval 无
  */
void AT_Protocol_DeviceStateFlag_Set(uint32_t device_state_flag);

/**
  * @brief 设备状态标志位清除  
  * @note   
  * @param  device_state_flag:
							AT_PROTOCOL_LOGIN_SEND_FLAG,AT_PROTOCOL_PORT_STATE_SEND_FLAG,AT_PROTOCOL_BEAT_SEND_FLAG,
							AT_PROTOCOL_BEAT_TIMEOUT_FLAG;
  * @retval 无
  */
void AT_Protocol_DeviceStateFlag_Clear(uint32_t device_state_flag);


/* 登录指令 发送标志位 */
#define AT_PROTOCOL_LOGIN_SEND_FLAG				0x01

/* 端口状态指令 发送标志位 */
#define AT_PROTOCOL_PORT_STATE_SEND_FLAG	0x02

/* 心跳指令 发送标志位 */
#define AT_PROTOCOL_BEAT_SEND_FLAG				0x04

/* 心跳超时检测标志位(离线检测) 主循环扫描此标志位即可 */
#define	AT_PROTOCOL_BEAT_TIMEOUT_FLAG			0x10



#endif

#ifndef __AT_PROTOCOL_H
#define __AT_PROTOCOL_H

/* 初次使用建议和CMD库数据手册一同使用, 带"选配"的字样, 代表可以传入 NULL */

/* 注意 头文件的宏修改了, 库内不会同步, 如果有特殊的容量需求, 将头文件的宏修改好后, 发给库开发者, 对库重新进行资源分配 */

/* 协议层句柄类型 */
typedef void * CMD_Protocol_Handle_t;

/* 协议层句柄数量 */
#define CMD_PROTOCOL_MEMPOOL_ELEMENT_COUNT	3

/* 每个句柄最大支持多少条协议指令 */
#define CMD_PROTOCOL_MENU_ELEMENT_COUNT			15

/* 每个句柄协议连包处理最大次数 */
#define CMD_PROTOCOL_LINK_BAG_MAX						3

/* 每个句柄协议发送任务队列大小 */
#define CMD_PROTOCOL_SEND_TASK_QUEUE_MAX		15


/* 协议菜单状态枚举 */
typedef enum
{
	CMD_PROTOCOL_OK = 0,				// 通用的成功状态
	CMD_PROTOCOL_ERR_PARAM,			// 错误参数
	CMD_PROTOCOL_ERR_FREE,			// 重复删除
	CMD_PROTOCOL_ERR_TIMEOUT,		// 错误超时
	
	CMD_PROTOCOL_STATE_FULL,		// 状态满
	CMD_PROTOCOL_STATE_IDLE,		// 状态空闲
	CMD_PROTOCOL_STATE_BUSY,		// 状态忙

}CMD_Protocol_State_t;



/* 协议层参数类型 */
typedef struct
{
	CMD_Core_Handle_t core_handle;			// 协议层使用到的核心层句柄类型
	uint32_t 					retry_threshold;	// 协议指令发送超时阈值, 周期是 CMD_Protocol_Task 函数的执行速度

	/* 
		1.当服务器下发的指令长度会超过模块接收上限
		2.模块是以<CR><LF>响应 
		满足上述俩种条件 packet_head_switch = 1, 否则 packet_head_switch = 0 
		满足上述俩种条件 module_recv_urc = 消息头, 例如 "+QIURC: \"recv\"", 否则 module_recv_urc = NULL 
	*/
	uint8_t			packet_head_switch;	
	const char *module_recv_urc;
	
	const char *module_send_done;																					// 模块将用户的数据发送完成后响应的数据, 例如 模块返回的 "SEND OK"
	uint16_t 		module_send_max_len;																			// 模块单次发送的最大数据量
	void			 (*module_send_func)(const char *string, uint32_t length);	// 发送定长数据函数
	
	const char *module_publish_cmd;															// 模块发送数据前的发布指令, 例如 "AT+QISEND=0,%d\r\n", "选配"
	const char *module_publish_prompt;													// 模块内部发送窗口打开响应的数据, 例如 用户发送完 "AT+QISEND=0,%d\r\n", 模块响应 '>', "选配"
	void 			 (*module_publish_func)(const char *string, ...);	// 格式符函数, 内部用来发送格式符数据, 例如 MQTT 主题, "选配"
	
}CMD_Protocol_Param_t;


/* 	
	联合体, 用户在使用发送队列入队函数的时候, 可以指定数据, 这个数据会传递到用户的 build 函数上,
不过在使用指针类型的时候要注意生命周期, 因为不是调用完入队函数就立马开始发送
*/
typedef union
{
	uint32_t u32;
	float		 f;
	void *	 ptr;
	void		(*func)(void);

}CMD_Protocol_SendQueue_Arg_t;



/**
  * @brief  协议层句柄创建
  * @note   只有创建句柄跟删除句柄需要传入二级指针, 其余接口函数可直接使用句柄操作
  * @param  handle: CMD_Protocol_Handle_t 类型的指针
  * @param  cfg:		CMD_Protocol_Param_t  类型的指针
  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM, CMD_PROTOCOL_STATE_FULL
  */
CMD_Protocol_State_t CMD_Protocol_Handle_Create(CMD_Protocol_Handle_t *handle, CMD_Protocol_Param_t *cfg);



/**
  * @brief  协议层句柄删除
  * @note   删除后的句柄将指向 NULL
  * @param  handle: CMD_Protocol_Handle_t 类型的指针
  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM, CMD_PROTOCOL_ERR_FREE
  */
CMD_Protocol_State_t CMD_Protocol_Handle_Delete(CMD_Protocol_Handle_t *handle);



/**
  * @brief  协议任务
  * @note   主循环扫描, 该函数会返回协议发送状态机的状态, 注意: 该函数返回 CMD_PROTOCOL_ERR_TIMEOUT 发送超时后, 超时的任务会保存, 转而进行重新发送, 所以用户可以让此函数在在线状态下运行, 超时后立马置网络状态为离线
  * @param  handle: CMD_Protocol_Handle_t 类型
  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM, CMD_PROTOCOL_STATE_BUSY, CMD_PROTOCOL_STATE_IDLE, CMD_PROTOCOL_ERR_TIMEOUT
  */
CMD_Protocol_State_t CMD_Protocol_Task(CMD_Protocol_Handle_t handle);



/**
  * @brief  添加协议指令函数

  * @note   使用方法：
							无需校验的数据: CMD_Protocol_Directive_Add(TT_LOGIN, TT_Login_Build, NULL, NULL);
							需校验的数据: 	CMD_Protocol_Directive_Add(TT_CTRL, TT_Ctrl_Build, TT_Ctrl_Recv, "\"msgType\":500");

  * @param  handle: CMD_Protocol_Handle_t 类型

	* @param  directive_index: 协议指令唯一标识, 范围 0 - CMD_PROTOCOL_MENU_ELEMENT_COUNT ;

	* @param  void(*build_func)(CMD_Protocol_SendQueue_Arg_t assign_data): 构建协议指令函数,示例代码如下;
							
							void TT_Login_Build(CMD_Protocol_SendQueue_Arg_t assign_data)
							{
								... 构建代码
								
								CMD_Protocol_SendPacket_Write(数据包起始地址指针, 数据包长度); // 将数据包起始地址跟长度传给发送任务状态机, 构建的任务就完成了(该值不可以是局部的);
							}
							注意: 无需指定数据的指令, 就不需要使用 assign_data, assin_data 是用户在调用协议发送队列入队函数中的第3个参数 ;

	* @param  char *(*parse_func)(char *addr): 接收平台回复(下发)指令, 示例代码如下; "选配"
							char *TT_Login_Recv(char *addr) { return NULL; }

							addr: directive_tag 首地址的指针, 出现 1 次 directive_tag, 就会调用 1 次用户的解析函数 parse_func ;
							
							char *返回值: 提前预留, 用户解析完如果需要特定操作可以返回 !NULL, 否则返回 NULL;

  * @param  directive_tag: 需要识别的指令标签,例如: "EBCharge", 无需校验则填 NULL, "选配"

  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM, CMD_PROTOCOL_STATE_FULL
  */
CMD_Protocol_State_t CMD_Protocol_Directive_Add(CMD_Protocol_Handle_t handle, uint32_t directive_index, void(*build_func)(CMD_Protocol_SendQueue_Arg_t assign_data), char *(*parse_func)(char *addr), char *directive_tag);







/**
  * @brief  协议发送队列入队
  * @note   主循环扫描, 建议网络在线后在调用
  * @param  handle: 				 CMD_Protocol_Handle_t 类型
  * @param  directive_index: 协议指令唯一标识
  * @param  assign_data:		 指定数据, 可以指定端口
  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM, CMD_PROTOCOL_STATE_FULL
  */
CMD_Protocol_State_t CMD_Protocol_SendQueue_Enqueue(CMD_Protocol_Handle_t handle, uint32_t directive_index, CMD_Protocol_SendQueue_Arg_t assign_data);



/**
	* @brief  协议发送状态机写数据包起始地址跟长度 函数
  * @note   在构建协议指令函数中, 构建完成后, 需要调用此函数将数据包起始地址跟长度传给发送状态机
  * @param  handle: CMD_Protocol_Handle_t 类型
  * @param  packet_addr:	数据包起始地址指针
  * @param  packet_len:		数据包长度
  * @retval CMD_Protocol_State_t: CMD_PROTOCOL_OK, CMD_PROTOCOL_ERR_PARAM
  */
CMD_Protocol_State_t CMD_Protocol_SendPacket_Write(CMD_Protocol_Handle_t handle, char *packet_addr, uint32_t packet_len);


#endif

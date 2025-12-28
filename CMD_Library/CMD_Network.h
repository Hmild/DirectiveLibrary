#ifndef __CMD_NETWORK_H
#define __CMD_NETWORK_H

/* 初次使用建议和CMD库数据手册一同使用, 带"选配"的字样, 代表可以传入 NULL */

/* 注意 头文件的宏修改了, 库内不会同步, 如果有特殊的容量需求, 将头文件的宏修改好后, 发给库开发者, 对库重新进行资源分配 */

/* 网络层句柄类型 */
typedef void * CMD_Network_Handle_t;

/* 网络层句柄数量 */
#define CMD_NETWORK_MEMPOOL_ELEMENT_COUNT					3

/* 每个句柄最大支持多少条指令 */
#define CMD_NETWORK_MENU_MEMPOOL_ELEMENT_COUNT		40



typedef enum
{
	CMD_NETWORK_OK = 0,			// 通用的成功状态

	CMD_NETWORK_ERR_PARAM,	// 错误参数
	CMD_NETWORK_ERR_FREE,		// 重复删除
	CMD_NETWORK_ERR_REPEAT,	// 指令唯一标识重复
	CMD_NETWORK_ERR_INDEX,	// 错误的唯一标识
	CMD_NETWORK_ERR_TIMEOUT,// 错误超时
	CMD_NETWORK_STATE_FULL,	// 状态满
	
}CMD_Network_State_t;


typedef struct
{
	CMD_Core_Handle_t	core_handle;	// 网络层使用到的核心层句柄

	uint32_t speed_threshold;		// 联网速度阈值, 单位是 CMD_Network_Task() 周期
	uint32_t timeout_threshold;	// 模块卡死阈值, 单位是 CMD_Network_Task() 周期
	uint8_t retry_threshold;		// 重试计数器阈值, 单位是 CMD_Network_Task() 周期

}CMD_Network_Param_t;


/**
  * @brief  网络层句柄创建
  * @note   只有创建句柄跟删除句柄需要传入二级指针, 其余接口函数可直接使用句柄操作
  * @param  handle: CMD_Network_Handle_t 类型的指针
  * @param  cfg: 		CMD_Network_Param_t 类型的指针
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_PARAM, CMD_NETWORK_STATE_FULL, 
  */
CMD_Network_State_t CMD_Network_Handle_Create(CMD_Network_Handle_t *handle, CMD_Network_Param_t *cfg);



/**
  * @brief  网络层句柄删除
  * @note   删除后的句柄将指向 NULL
  * @param  handle: CMD_Network_Handle_t 类型的指针
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_PARAM, CMD_NETWORK_ERR_FREE
  */
CMD_Network_State_t CMD_Network_Handle_Delete(CMD_Network_Handle_t *handle);




/**
  * @brief  联网状态机任务
  * @note   未连接(离线)状态下, 定时执行, 建议300ms, 由该函数扫描周期和 speed_threshold 决定
  * @param  handle: CMD_Network_Handle_t 类型
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_TIMEOUT
  */
CMD_Network_State_t CMD_Network_Task(CMD_Network_Handle_t handle);


/**
  * @brief  网络指令添加(最多 CMD_NETWORK_MENU_MEMPOOL_ELEMENT_COUNT 条)范围: 指令数量 < CMD_NETWORK_MENU_MEMPOOL_ELEMENT_COUNT
  * @note   使用方法: CMD_Network_Directive_Add(handle, TT_CSQ, TT_Send_CSQ, TT_Recv_CSQ);
  * @param  handle: CMD_Core_Handle_t 类型
  * @param  directive_index: 网络指令的唯一标识
  * @param  send_func: 	联网指令唯一标识对应的发送函数,例如 CSQ 那么该函数就是实现 AT+CSQ 指令
  * @param  check_func: 联网指令唯一标识对应的校验函数,例如 CSQ 那么该函数就是校验 AT+CSQ 指令模块返回的数据
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_PARAM, CMD_NETWORK_ERR_REPEAT, CMD_NETWORK_STATE_FULL
  */
CMD_Network_State_t CMD_Network_Directive_Add(CMD_Network_Handle_t handle, uint32_t directive_index, void(*send_func)(void), char *(check_func)(void));

/**
  * @brief  网络指令删除
  * @note   使用方法: CMD_Network_Directive_Delete(handle, TT_CSQ);
  * @param  handle: CMD_Core_Handle_t 类型
  * @param  directive_index: 网络指令的唯一标识
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_PARAM, CMD_NETWORK_ERR_INDEX
  */
CMD_Network_State_t CMD_Network_Directive_Delete(CMD_Network_Handle_t handle, uint32_t directive_index);



/**
  * @brief  网络连接起始指令设置
  * @note   每个网络指令在添加的时候都会有个唯一标识号
  * @param  handle: CMD_Core_Handle_t 类型
	* @param  start: 网络起始连接指令,唯一标识
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_INDEX
  */
CMD_Network_State_t CMD_Network_StartLinkDirective_Set(CMD_Network_Handle_t handle, uint32_t start);

/**
  * @brief 	网络连接当前指令设置
  * @note   用户可以在接收发送AT命令函数内自行修改当前联网步骤
  * @param  handle: CMD_Core_Handle_t 类型
  * @param  current: 当前连接的起始指令,唯一标识
  * @retval CMD_Network_State_t: CMD_NETWORK_OK, CMD_NETWORK_ERR_INDEX
  */
CMD_Network_State_t CMD_Network_CurrentLinkDirective_Set(CMD_Network_Handle_t handle, uint32_t current);



#endif

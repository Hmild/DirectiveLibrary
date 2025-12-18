#ifndef __AT_NETWORK_H
#define __AT_NETWORK_H

/* 初次使用建议和AT库数据手册一同使用 */


/* 网络菜单内存池容量 */
#define AT_NETWORK_MENU_MEMPOOL_SIZE		30


/* 初始化内存池(初始化调用1次即可) */
void AT_Network_Menu_MempoolInit(void);


/* 联网状态机任务函数, AT_NETWORK_OFF 执行,每 speed_threshold 执行1次(定时执行1次,300ms以上) */
void AT_Network_Task(void);


typedef enum
{
	/* 添加网络指令返回的状态 */
	AT_NETWORK_MENU_RAM_ERROR = 0,
	AT_NETWORK_MENU_PARAM_DEFAULT,
	AT_NETWORK_MENU_DIRECTIVE_INDEX_REPEAT,
	
	/* 删除(设置)网络指令返回的状态 */
	AT_NETWORK_MENU_NO_FOUND_INDEX,

	/* 共用的成功状态 */
	AT_NETWORK_MENU_SUCCEED,

}AT_Network_MenuState_t;



/**
  * @brief  添加网络指令(最多 AT_NETWORK_MENU_MEMPOOL_SIZE 条)
  * @note   使用方法: AT_Network_Directive_Add(TT_CSQ, TT_Send_CSQ, TT_Recv_CSQ);
  * @param  directive_index: 网络指令的唯一标识,用户自行定义,范围: < AT_NETWORK_MENU_MEMPOOL_SIZE
  * @param  send_command: 联网指令索引对应的发送函数,例如 CSQ 那么该函数就是实现 AT+CSQ 指令
  * @param  recv_command: 联网指令索引对应的接收函数,例如 CSQ 那么该函数就是校验 AT+CSQ 指令模块返回的数据
  * @retval AT_Network_MenuState_t: 网络菜单状态
  */
AT_Network_MenuState_t AT_Network_Directive_Add(uint32_t directive_index, void(*send_command)(void), char *(recv_command)(void));

/**
  * @brief  删除网络指令
  * @note   使用方法: AT_Network_Directive_Delete(TT_CSQ);
  * @param  directive_index: 网络指令的唯一标识
  * @retval AT_Network_MenuState_t: 网络菜单状态
  */
AT_Network_MenuState_t AT_Network_Directive_Delete(uint32_t directive_index);



/**
  * @brief  网络连接起始指令设置
  * @note   每个网络指令在添加的时候都会有个唯一标识号,是用户自定义的,传入 TT_CSQ ,代表从 TT_CSQ 对应的指令开始连接
	* @param  start: 网络起始连接指令
  * @retval AT_Network_LinkDirective_t: 网络连接索引设置状态
  */
AT_Network_MenuState_t AT_Network_StartLinkDirective_Set(uint32_t start);

/**
  * @brief 	网络连接当前指令设置
  * @note   用户可以在接收发送AT命令函数内自行修改当前联网步骤,设置 TT_CSQ ,下一条将执行 TT_CSQ 对应的指令
  * @param  current: 当前连接的起始指令索引, AT_Network_Cmd_t 类型
  * @retval AT_Network_LinkDirective_t: 网络连接索引设置状态
  */
AT_Network_MenuState_t AT_Network_CurrentLinkDirective_Set(uint32_t current);





/* AT_NETWORK_OFF :离线 */
#define AT_NETWORK_OFF		0

/* AT_NETWORK_ON :在线 */
#define AT_NETWORK_ON			1


/* 用户需要创建一个 AT_Network_t 类型的全局联网状态标志,进行离线,在线区分 */
typedef struct
{
	uint8_t network_flag; 	// 联网标志位,对应 AT_NETWORK_OFF , AT_NETWORK_ON
	uint8_t iccid[25];
	uint8_t imei[20];
	
}AT_Network_t;



typedef struct
{
	uint32_t speed_threshold;		// 联网速度阈值,单位是 AT_Network_Task() 周期,默认值 300
	uint32_t timeout_threshold;	// 联网超时阈值,单位是 AT_Network_Task() 周期,默认值 7000
	uint8_t retry_threshold;		// 重试计数器阈值,单位是 AT_Network_Task() 周期,默认值 3

}AT_Network_Param_t;

/* 网络参数配置 */
void AT_Network_ParamConfig(AT_Network_Param_t *network_param_cfg);



/**
	* @brief  从 value 最低位开始寻找 0
  * @note   位图方法,兼容不同平台,内部内存池分配会使用
	* @param  value: 32位的变量,不可以传入 0xFFFFFFFFU
  * @retval 0在value的位置
  */
static inline int Find_FirstZero(uint32_t value)
{
	if(value == 0xFFFFFFFFU) return -1;

	#if defined(__GNUC__) || defined(__clang__)
		return __builtin_ctz(~value);

	#else
		for(uint8_t i=0; i<32; i++)
		{
			if((value & (1U << i)) == 0) return i;
		}
		return -1;

	#endif
}


#endif

#ifndef __CMD_CORE_H
#define __CMD_CORE_H

/* 初次使用建议和CMD库数据手册一同使用, 带"选配"的字样, 代表可以传入 NULL */

/* 注意 头文件的宏修改了, 库内不会同步, 如果有特殊的容量需求, 将头文件的宏修改好后, 发给库开发者, 对库重新进行资源分配 */

/* 核心层句柄类型 */
typedef void * CMD_Core_Handle_t;

/* 核心层句柄数量 */
#define CMD_CORE_SERIAL_MEMPOOL_ELEMENT_COUNT		3



typedef enum
{
	CMD_CORE_OK,					// 通用的成功状态
	CMD_CORE_ERR_PARAM,		// 错误参数
	CMD_CORE_ERR_FREE,		// 重复删除
	CMD_CORE_STATE_FULL,	// 状态满

}CMD_Core_State_t;


typedef struct
{
	char *recv_buf;						// DMA 目的缓冲区首地址指针
	char *recv_line_buf;			// DMA 环形缓冲区首地址指针

	uint16_t recv_buf_size;			// DMA 搬运目的缓冲区, 环形缓冲区长度(这俩个缓冲区大小需一致)
	uint32_t scan_threshold;		// 串行口接收超时扫描阈值
	uint32_t timeout_threshold;	// 串行口接收超时阈值

	uint16_t (*dma_count_get)(void); // DMA 通道传输计数器获取

}CMD_Core_SerialParam_t;



/**
  * @brief  核心层句柄创建
  * @note   只有创建句柄跟删除句柄需要传入二级指针, 其余接口函数可直接使用句柄操作
  * @param  handle: CMD_Core_Handle_t 类型的指针
  * @param  cfg:		CMD_Core_SerialParam_t 类型的指针
  * @retval CMD_Core_State_t: CMD_CORE_OK, CMD_CORE_ERR_PARAM, CMD_CORE_STATE_FULL
  */
CMD_Core_State_t CMD_Core_SerialHandle_Create(CMD_Core_Handle_t *handle, CMD_Core_SerialParam_t *cfg);


/**
  * @brief  核心层句柄删除
  * @note   删除后的句柄将指向 NULL
  * @param  handle: CMD_Core_Handle_t 类型的指针
  * @retval CMD_Core_State_t: CMD_CORE_OK, CMD_CORE_ERR_PARAM, CMD_CORE_ERR_FREE
  */
CMD_Core_State_t CMD_Core_SerialHandle_Delete(CMD_Core_Handle_t *handle);



/**
  * @brief  串行口接收超时任务
  * @note   建议放入1ms执行1次的任务区
  * @param  handle: CMD_Core_Handle_t 类型的指针
  * @retval CMD_Core_State_t: CMD_CORE_OK, CMD_CORE_ERR_PARAM
  */
CMD_Core_State_t CMD_Core_SerialRecvTimeout_Task(CMD_Core_Handle_t handle);



#endif

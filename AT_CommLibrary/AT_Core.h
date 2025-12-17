#ifndef __AT_CORE_H
#define __AT_CORE_H

/* 初次使用建议和AT库数据手册一同使用 */

typedef struct
{
	uint8_t *recv_buf;			// DMA 外设到存储器模式下目的缓冲区首地址指针
	uint8_t *recv_line_buf;	// DMA 环形缓冲区首地址指针
	uint8_t *temp_buf;			// 可变参数函数发送缓冲区, 发送任务状态机寻找 '>', "OK" 缓冲区首地址指针

	uint16_t recv_buf_size;	// DMA 搬运目的缓冲区, 环形缓冲区长度(这俩个缓冲区大小需一致),用户自行决定长度
	uint32_t temp_buf_size;	// 可变参数函数发送缓冲区, 发送任务状态机寻找 '>', "OK" 缓冲区,用户自行决定长度

}AT_Core_SerialBuf_t;

/* 传入 AT_Core_SerialBuf_t 类型的变量配置内部缓冲区指针 */
void AT_Core_SerialBuf_Config(AT_Core_SerialBuf_t *serial_buf_cfg);



/* 设置串行口接收超时扫描周期,默认值 1 */
void AT_Core_SerialRecv_SM_ScanThreshold_Set(uint32_t threshold);

/* 设置串行口接收超时阈值,默认值 3 */
void AT_Core_SerialRecv_SM_TimeoutThreshold_Set(uint32_t threshold);

/* 串口接收超时扫描函数,建议放入1ms溢出1次的定时器中断,可以通过调整,串行口接收超时扫描周期,延长执行1次的时间 */
void AT_Core_SerialRecvTimeout_Task(void);




/* 用户在使用下面的功能函数之前,需要先注册好 AT_HardwarePort.h 硬件接口层 */

/* 发送字节 接口函数 */
void AT_Core_SendByte(uint8_t byte);

/* 发送字符串 接口函数 */
void AT_Core_SendString(const char *string);

/* 发送定长数据 接口函数 */
void AT_Core_SendFixedLengthData(const char *string, uint32_t length);

/* 发送可变参数 接口函数 */
int AT_Core_Printf(const char *string, ...);


/* 模块开关机 接口函数 */
void AT_Core_ModulePower(void);

/* 获取DMA传输计数器剩余数量 接口函数 */
uint16_t AT_Core_DMA_TransferNumber_Get(void);

/* 返回 DMA 环形缓冲区中第一次出现 string 位置的指针,未找到则返回 NULL */
char *AT_Core_RecvBuf_Check(const char *string);




#endif

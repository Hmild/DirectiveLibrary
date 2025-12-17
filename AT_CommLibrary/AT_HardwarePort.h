#ifndef __AT_HARDWAREPORT_H
#define __AT_HARDWAREPORT_H

/* 初次使用建议和AT库数据手册一同使用 */


typedef enum
{
	AT_HW_REGISTER_ERR=0,
	AT_HW_REGISTER_SUCCEED,

}AT_HW_RegisterState_t;

typedef struct
{
	void(*send_byte)(uint8_t byte);	
	void(*wait_data_send_finish)(void);
	void(*module_power)(void);
	uint16_t(*dma_transfer_number_get)(void);

}AT_HW_Port_t;

/**
  * @brief  注册硬件接口
  * @note   用户提供 AT_HW_Port_t 结构体类型的成员
  * @param  hw_port_cfg: AT_HW_Port_t 类型的指针
  * @retval AT_HW_RegisterState_t 类型的状态
  */
AT_HW_RegisterState_t AT_HW_Port_Register(AT_HW_Port_t *hw_port_cfg);



#endif

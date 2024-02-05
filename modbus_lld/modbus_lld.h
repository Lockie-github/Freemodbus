#ifndef _MODBUS_LLD_H_
#define _MODBUS_LLD_H_

/* ----------------------- System includes --------------------------------*/
#include <stdint.h>

/* ----------------------- Platform includes --------------------------------*/
// #include "self_uart.h"
// #include "self_timer.h"

#include "usart.h"
#include "tim.h"
#include "main.h"
#include "stm32h7xx_hal.h"



#include "modbus_port.h"



/*-----------UART--------------*/

void vMBLLDUartInit(uint32_t ulBaudRate, uint8_t eParity );

void vMBLLDReceiveEnable(void);

void vMBLLDSendEnable(void);

void vMBLLDPutByte(uint8_t ucbyte);

void vMBLLDGetByte(uint8_t *pucbyte);

/*-----------TIMER--------------*/

void vMBLLDTimerInit(uint16_t usTim1Timerout50us);

void vMBLLDTimerEnable(void);

void vMBLLDTimerDisable(void);

#endif
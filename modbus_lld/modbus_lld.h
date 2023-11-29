#ifndef _MODBUS_LLD_H_
#define _MODBUS_LLD_H_

/* ----------------------- System includes --------------------------------*/
#include <stdint.h>

/* ----------------------- Platform includes --------------------------------*/
// #include "self_uart.h" 
// #include "self_timer.h"



/*-----------UART--------------*/

void vMBLLDUartInit(void);

void vMBLLDReceiveEnable(void);

void vMBLLDSendEnable(void);

void vMBLLDPutByte(uint8_t ucbyte);

void vMBLLDGetByte(uint8_t *pucbyte);

/*-----------TIMER--------------*/

void vMBLLDTimerInit(uint16_t usTim1Timerout50us);

void vMBLLDTimerEnable(void);

void vMBLLDTimerDisable(void);

#endif
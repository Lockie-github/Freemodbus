#include "modbus_lld.h"

void vMBLLDUartInit(uint32_t ulBaudRate, uint8_t eParity )
{
    // MX_USART1_UART_Init(ulBaudRate);
}

void vMBLLDReceiveEnable(void)
{

    // HAL_UART_Receive_IT(&huart1,(uint8_t *)&uart_recv_buf[uart_recv_cnt],1);
    
}

void vMBLLDSendEnable(void)
{


}

void vMBLLDPutByte(uint8_t ucByte)
{ 

    // HAL_UART_Transmit_IT(&huart1,&ucByte,1);

}

void vMBLLDGetByte(uint8_t *pucByte)
{
    *pucByte = uart_recv_buf[uart_recv_cnt-1];

}



void vMBLLDTimerInit(uint16_t usTim1Timerout50us)
{

    // MX_TIM1_Init(usTim1Timerout50us);

}

void vMBLLDTimerEnable(void)
{
    // __HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE); //清一下计数值
    // HAL_TIM_Base_Start_IT(&htim1);    

}

void vMBLLDTimerDisable(void)
{	
    // HAL_TIM_Base_Stop_IT(&htim1);
}


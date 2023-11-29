#include "modbus_lld.h"

/**
 * @brief uart初始化函数
 * @note 调用自定义的初始化函数即可
 * 
 */
void vMBLLDUartInit(void)
{
	// uart_init();
}

/**
 * @brief 接收使能
 * @note 此处主要是为了用于RS485系统的芯片的接收使能
 * 
 */
void vMBLLDReceiveEnable(void)
{
    // nrf_gpio_pin_clear(MODBUS_RE);
}

/**
 * @brief 发送使能
 * @note 此处主要是为了用于RS585系统的芯片发送使能
 * 
 */
void vMBLLDSendEnable(void)
{
    // nrf_gpio_pin_set(MODBUS_RE);
}

/**
 * @brief 发送一个字节
 * @note 发送一个字节的实现
 * @param ucByte 要发送的字节
 */
void vMBLLDPutByte(uint8_t ucByte)
{
    // app_uart_put(ucByte); 
}

/**
 * @brief 接受一个字节
 * @note 接受一个字节的实现
 * @param pucByte 接收数据缓存地址
 */
void vMBLLDGetByte(uint8_t *pucByte)
{
    // *pucByte = uart_recv_buf[uart_recv_cnt-1];
}


/**
 * @brief 定时器初始化
 * 
 * @param usTim1Timerout50us T3.5的计数值,根据时钟频率和波特率计算
 */
void vMBLLDTimerInit(uint16_t usTim1Timerout50us)
{
    // timer_init(usTim1Timerout50us);
}

/**
 * @brief 使能定时器
 * 
 */
void vMBLLDTimerEnable(void)
{
    // timer_enable();
}

/**
 * @brief 禁用定时器
 * 
 */
void vMBLLDTimerDisable(void)
{	
    // timer_disable();
}


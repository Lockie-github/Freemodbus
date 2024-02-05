### 移植说明(RTU或ASCII)
#### 创建一个定时器
Modbus协议栈需要一个定时器来检测帧的结束。定时器的分辨率应为串行字符时间的一半。例如，对于 38400 波特，假设单个字符为 11 位，字符时间约为280us。协议栈使用的最小超时是字符超时的3.5倍。在/rtu/mbrtu.c的`eMBRTUInit`函数中根据定时器配置信息修改`usTimerT35_50us`的计算公式,确认定时器工作正常后,在定时器的回调函数中需要调用port/porttimer.c中的`prvvTIMERExpiredISR`函数
#### 创建一个串口
同样的,需要创建一个uart来接收和发送数据,确认uart的中断收发正常后,在接收中断中调用port/portserial.c中的`prvvUARTRxISR`函数,在发送中断中调用port/portserial.c中的`prvvUARTTxReadyISR`函数
#### 完善modbuslld
确保定时器和uart功能正常后,完善modbus_lld/modbus_lld.c中的接口,该部分需要根据芯片的不同自行定义,注意`vMBLLDGetByte`函数的实现
#### 配置寄存器
首先确保port/modbus_port.h中的`ENTER_CRITICAL_SECTION`和`EXIT_CRITICAL_SECTION`工作正常,然后根据需求配置相关寄存器的地址和数量
#### 创建modbus线程
```c
static void start_freemosbus_task(void *param)
{
    eMBInit(MB_RTU, 0x01, 1, 9600, MB_PAR_NONE);  //初始化FreeModbus
    
    eMBEnable();  //启动FreeModbus

    while (1)
    {  
        (void)eMBPoll();  //查询数据帧
        vTaskDelay(100);
    }
    
}
```
### modbus 协议
#### ADU
| ID | function | data | CRC |
| --- | --- | --- | --- |
| 8bit| 8bit|n*8bit|16bit|
#### PDU
| 功能码 | 寄存器地址 | 寄存器个数 |
| -     | -        | -         |
| 1byte | 2byte    | 2byte     |

#### function table
| function | description | comment |
| -        | -           | -       |
| 0x01     | 读线圈       |         |
| 0x02     | 读离散输入    |        |
| 0x03     | 读取保持寄存器 |        |
| 0x04     | 读输入寄存器  |        |
| 0x05     | 写单个线圈    |        |
| 0x06     | 写单个寄存器 |         |
| 0x07     | 读异常状态   | NULL    |
| 0x08     | 诊断        | NULL    | 
| 0x10     | 写多个寄存器 |
| 0x11     | 报告从机地址 |         |
| 0x14     | 读取文件记录 | NULL    |
| 0x17     |   读/写多个寄存器 |    |

#### 寄存器定义
| Reg name | description | comment |
| -        | -           | -       |
| usRegHoldingBuf | 保持寄存器,存放可读写的数据 | |
| usRegInputBuf | 输入寄存器,用于存放只读的数据 | 
| usRegCOIlSBuf | 线圈寄存器 | |
| usRegDISCBuf | 离散量寄存器 | |

#### example
read usRegInputBuf 
| 01 | 04 | 00 | 00 | 00 | 04 | CRCL | CRCH |
| --- | --- | --- | --- | --- | --- | --- | --- |
|设备地址 | 功能码 | 寄存器起始地址高 |寄存器起始地址低位 | 寄存器数量高位 | 寄存器数量低位 | CRC低位 | CRC高位 |
    respond:01 04 04 00 00 00 00 00 00 00 00 CRCL CRCH
    01(设备地址) 04(功能码) 04(字节数) 0000 0000 0000 0000(读到的寄存器数据)
    
read usRegHoldingBuf
| 01 | 03 | 00 | 00 | 00 | 04 | CRCL | CRCH |
| --- | --- | --- | --- | --- | --- | --- | --- |
|设备地址 | 功能码 | 寄存器起始地址高 |寄存器起始地址低位 | 寄存器数量高位 | 寄存器数量低位 | CRC低位 | CRC高位 |

write single usRegHoldingBuf
| 01 | 06 | 00 | 00 | 00 | 04 | CRCL | CRCH |
| --- | --- | --- | --- | --- | --- | --- | --- |
|设备地址 | 功能码 | 寄存器起始地址高 |寄存器起始地址低位 | 写入数据高位 | 写入数据低位 | CRC低位 | CRC高位 |

write multiple usRegHoldingBuf
| 01 | 10 | 00 | 00 | 00 | 04 | 04 | 00 | 00 | ... | 00 | 00 | CRCL | CRCH |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|设备地址 | 功能码 | 寄存器起始地址高 |寄存器起始地址低位 | 寄存器数量高位 | 寄存器数量低位 | 字节数 | 数据1高位 | 数据1低位 | ... | 数据N高位 | 数据N低位 | CRC低位 | CRC高位 |

#### circuit
* 首先完成物理层设备的初始化,主要包括串口和定时器
* 然后使能协议栈、串口和定时器
* 在第一个T35中断来临后,协议栈进入接收状态
* 此时调用eMBPoll检测事件
    * 若发送接收中断,则不断向缓存中存入接收的数据,清空定时器计数值,并等待T35的到来
    * 根据Modbus协议规定,若T35中断产生,则标志一帧数据接收完成,向协议栈发送EV_FRAME_RECEIVED事件
* eMBPoll检测到EV_FRAME_RECEIVED事件后,会调用函数简单判断数据帧,并发送EV_EXECUTE事件
* eMBPoll检测到EV_EXECUTE事件后,会根据功能码处理请求并将回复的数据复制到缓存中,将发送状态设置为STATE_TX_XMIT,然后使能发送中断
    * 若发送状态为STATE_TX_XMIT,则串口会将缓存中的数据发送出去,完成后向协议栈发送EV_FRAME_SENT事件
* eMBPoll检测到EV_FRAME_SENT事件后,不处理该事件并开始等待接收中断

### API description
#### port
| file | API | description |
| -    | -   | -           |
| portevent.c |  |  |
|  | xMBPortEventInit | 创建任务队列 |
|  | xMBPortEventPost |发布任务到队列 |
|  | xMBPortEventGet |获取任务 |
| portserial.c |  |  |
|  | xMBPortSerialInit | 初始化串口配置 |
|  | vMBPortSerialEnable | 配置串口发送&接收中断,在nrf52X中主要是配置RS485系统的收发 |
|  | xMBPortSerialPutByte | 发送一个字符 |
|  | xMBPortSerialGetByte | 接收一个字符 |
|  | prvvUARTTxReadyISR | 发送中断,此函数无需修改,只需在用户发送中断中调用,但是需要注意调用该函数后清除中断标志位 |
|  | prvvUARTRxISR | 接收中断,此函数无需修改,只需在用户接收中断中调用,但是需要注意调用该函数后清除中断标志位 |
| porttimer.c |  |  |
|  | xMBPortTimersInit | 初始化定时器,此函数无需修改,只需在在LLD中实现即可 |
|  | vMBPortTimersEnable | 使能定时器,此函数无需修改,只需在在LLD中实现即可 | 
|  | vMBPortTimersDisable | 关闭定时器中断,此函数无需修改,只需在在LLD中实现即可 |

#### modbus rtu
| file | API | description |
| -    | -   | -           |
| mbcrc.c |  |  |
|  | usMBCRC16 | 获取CRC |
| mbrtu.c |  |  |
| eMBRTUInit | 初始化串口和定时器 |
| eMBRTUStart | 修改modbus状态,使能接收&定时器 |
| eMBRTUStop | 关闭接收、发送和定时器 |
| eMBRTUReceive | 提取接收到的数据帧中的PDU并传递PDU的地址和长度 |
| eMBRTUSend | 拼接要发送的帧,并修改modbus状态发送状态 | 
| xMBRTUReceiveFSM | 接收状态机 |
| xMBRTUTransmitFSM | 发送状态机 |
| xMBRTUTimerT35Expired | T3.5 |

#### mb.c
| file | API | description |
| -    | -   | -           |
|  | eMBInit | Modbus协议栈初始化 |
|  | eMBEnable | Modbus协议开始 |
|  | eMBDisable |Modbus协议终止 | 
|  | eMBPoll | 事件轮询函数,通过返回的事件决定要进行的操作 |
|  | eMBRegisterCB | Modbus功能注册函数,该函数可以注册modbus协议外的功能码 |





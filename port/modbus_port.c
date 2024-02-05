 /*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id: port.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
  */

/* ----------------------- System includes ----------------------------------*/

/* ----------------------- Platform includes --------------------------------*/
// #include "nrf_log.h"

/* ----------------------- Modbusport includes -----------------------------*/
#include "modbus_port.h"

/* ----------------------- Modbus includes ---------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

/* ----------------------- Variables ----------------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
// void
// EnterCriticalSection(  )
// {
// 	__ASM volatile("cpsid i");
// }

// void
// ExitCriticalSection(  )
// {
//     __ASM volatile("cpsie i");
// }





/******************************************************************************
                               定义Modbus相关参数
******************************************************************************/
///< 输入寄存器 RO
USHORT usRegInputStart = REG_INPUT_START;
USHORT usRegInputBuf[REG_INPUT_NREGS];

///< 保持寄存器 RW
USHORT usRegHoldingStart = REG_HOLDING_START;
USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

///< 线圈寄存器 RW
USHORT usRegCOILSStart = REG_COILS_START;
UCHAR usRegCOIlSBuf[REG_COILS_NREGS];

///< 离散量寄存器 RO
USHORT usRegDISCStart = REG_DISC_START;
UCHAR usRegDISCBuf[REG_DISC_NREGS];

/**
  *****************************************************************************
  * @Name   : 操作输入寄存器
  *
  * @Brief  : 对应功能码0x04 -> eMBFuncReadInputRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;

	/* it already plus one in modbus function method. */
    usAddress--;	
	
	//判断地址合法性
	//为了避免警告，修改为有符号整数
	if (((SHORT)usAddress >= REG_INPUT_START) && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
	{
		iRegIndex = (int)(usAddress - usRegInputStart);
		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] >> 8);  //高8位字节
			*pucRegBuffer++ = (UCHAR)( usRegInputBuf[iRegIndex] & 0xFF); //低8位字节
			iRegIndex++;
			usNRegs--;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作保持寄存器
  *
  * @Brief  : 对应功能码0x06 -> eMBFuncWriteHoldingRegister
  *                    0x16 -> eMBFuncWriteMultipleHoldingRegister
  *                    0x03 -> eMBFuncReadHoldingRegister
  *                    0x23 -> eMBFuncReadWriteMultipleHoldingRegister
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int          iRegIndex = 0;

	/* it already plus one in modbus function method. */
    usAddress--;

	//判断地址是否合法
	//为了避免警告，修改为有符号整数
	if(((SHORT)usAddress >= REG_HOLDING_START) && ((usAddress + usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);

		/* it already plus one in modbus function method. */
    	usAddress--;

		//根据功能码进行操作
		//
		switch(eMode)
		{
			case MB_REG_READ:  //读保持寄存器
					while(usNRegs > 0)
					{
						*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);  //高8位字节
						*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); //低8位字节
						iRegIndex++;
						usNRegs--;
					}                            
					break;
					
			case MB_REG_WRITE:  //写保持寄存器
					while(usNRegs > 0)
					{
						usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;  //高8位字节
						usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;  //低8位字节
						iRegIndex++;
						usNRegs--;
					}
					break;
		}
	}
	else  //错误地址
	{
		eStatus = MB_ENOREG;
	}
	
	return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作线圈
  *
  * @Brief  : 对应功能码0x01 -> eMBFuncReadCoils
  *                    0x05 -> eMBFuncWriteCoil
  *                    0x15 -> 写多个线圈 eMBFuncWriteMultipleCoils
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *           eMode:         功能码
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    iNReg =  usNCoils / 8 + 1;

    /* it already plus one in modbus function method. */
    usAddress--;

    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + COIL_NCOILS ) )
    {
        iRegIndex = (USHORT) (usAddress - REG_COILS_START) / 8;
        iRegBitIndex = (USHORT) (usAddress - REG_COILS_START) % 8;
        switch ( eMode )
        {
        /* read current coil values from the protocol stack. */
        case MB_REG_READ:
            while (iNReg > 0)
            {
                *pucRegBuffer++ = xMBUtilGetBits(&usRegCOIlSBuf[iRegIndex++],
                        iRegBitIndex, 8);
                iNReg--;
            }
            pucRegBuffer--;
            /* last coils */
            usNCoils = usNCoils % 8;
            /* filling zero to high bit */
            *pucRegBuffer = *pucRegBuffer << (8 - usNCoils);
            *pucRegBuffer = *pucRegBuffer >> (8 - usNCoils);
            break;

            /* write current coil values with new values from the protocol stack. */
        case MB_REG_WRITE:
            while (iNReg > 1)
            {
                xMBUtilSetBits(&usRegCOIlSBuf[iRegIndex++], iRegBitIndex, 8,
                        *pucRegBuffer++);
                iNReg--;
            }
            /* last coils */
            usNCoils = usNCoils % 8;
            /* xMBUtilSetBits has bug when ucNBits is zero */
            if (usNCoils != 0)
            {
                xMBUtilSetBits(&usRegCOIlSBuf[iRegIndex++], iRegBitIndex, usNCoils,
                        *pucRegBuffer++);
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
  *****************************************************************************
  * @Name   : 操作离散寄存器
  *
  * @Brief  : 对应功能码0x02 -> eMBFuncReadDiscreteInputs
  *
  * @Input  : *pucRegBuffer：数据缓冲区，响应主机用
  *           usAddress:     寄存器地址
  *           usNRegs:       操作寄存器个数
  *
  * @Output : none
  *
  * @Return : Modbus状态信息
  *****************************************************************************
**/
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          iRegIndex , iRegBitIndex , iNReg;
    iNReg =  usNDiscrete / 8 + 1;

    /* it already plus one in modbus function method. */
    usAddress--;

    if ((usAddress >= usRegDISCStart)
            && (usAddress + usNDiscrete    <= usRegDISCStart + DISC_NDISC))
    {
        iRegIndex = (USHORT) (usAddress - usRegDISCStart) / 8;
        iRegBitIndex = (USHORT) (usAddress - usRegDISCStart) % 8;

        while (iNReg > 0)
        {
            *pucRegBuffer++ = xMBUtilGetBits(&usRegDISCBuf[iRegIndex++],
                    iRegBitIndex, 8);
            iNReg--;
        }
        pucRegBuffer--;
        /* last discrete */
        usNDiscrete = usNDiscrete % 8;
        /* filling zero to high bit */
        *pucRegBuffer = *pucRegBuffer << (8 - usNDiscrete);
        *pucRegBuffer = *pucRegBuffer >> (8 - usNDiscrete);
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

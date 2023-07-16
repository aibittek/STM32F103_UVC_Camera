/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "platform_config.h"
#include "usb_pwr.h"
#include "lcd.h"
#include "srcjpg.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


//配置USB时钟,USBclk=48Mhz
void Set_USBClock(void)
{
	RCC->CFGR &= ~(1 << 22); //USBclk=PLLclk/1.5=48Mhz	    
	RCC->APB1ENR |= 1 << 23; //USB时钟使能					 
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;
	/* Request to enter STOP mode with regulator in low power mode */
	//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;


	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	} else
	{
		bDeviceState = ATTACHED;
	}
}


// 测试部分
#include "srcjpg.h"
//UVC payload head

#define CAMERA_SIZ_STREAMHD			2
u8 sendbuf[PACKET_SIZE] = { 0x02, 0x01 };			// 发送数据缓冲区
u32 sendsize = 0;									// 已发送字节数

void myMemcpy(const u8* src, u8* dst, u32 len)
{
	u32 i = 0;
	for (i = 0; i < len; ++i)
	{
		dst[i] = src[i];
	}
}

//USB中断配置
void USB_Interrupts_Config(void)
{

	EXTI->IMR |= 1 << 18;//  开启线18上的中断
	EXTI->RTSR |= 1 << 18;//line 18上事件上升降沿触发	 
	MY_NVIC_Init(1, 0, USB_LP_CAN1_RX0_IRQn, 2);//组2，优先级次之 
	MY_NVIC_Init(0, 0, USBWakeUp_IRQn, 2);     //组2，优先级最高	 	 
}

///*******************************************************************************
//* Function Name : UsbCamera_Send.
//* Description   : 初始化编码器
//* Input         : 
//* Output        : 
//* Return value  : 
//*******************************************************************************/
//void UsbCamera_StreamStart(void)
//{
//	//u32 datalen = 0;
//
//	//初始化UVC payload head
//	sendbuf[0] = 0x02;		//HLF为2(标头长度字段指定了标头的长度，单位为字节)
//	sendbuf[1] = 0x01;	
//	// 初始化单帧数据传输计数
//	sendsize = 0;
//
//	//// 计算发送数据长度
//	//datalen = PACKET_SIZE - 2;
//	//myMemcpy(sbuf, sendbuf + 2, datalen);
//	//UserToPMABufferCopy(sendbuf, ENDP1_BUF0Addr, PACKET_SIZE);
//	//_SetEPDblBuf0Count(ENDP1, EP_DBUF_IN, PACKET_SIZE);
//	//sendsize = datalen;
//
//	// 收到IN令牌后发送数据
//	_SetEPTxStatus(ENDP1, EP_TX_VALID);
//}

/*******************************************************************************
* Function Name  : UsbCamera_Fillbuf
* Description    : 准备待发送的视频流缓冲区
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void UsbCamera_Fillbuf(void)
{
	s32 datalen = 0;		// 本次发送的字节数
	u8 *payload = 0;		// 发送数据指针

	// 发送缓冲区有效数据地址
	payload = sendbuf + CAMERA_SIZ_STREAMHD;
	// 读数据到发送缓冲区
	if (0 == sendsize)
	{
		sendbuf[1] &= 0x01;		// 清除BFH
		sendbuf[1] ^= 0x01;		// 切换FID

		// 计算本次发送数据长度
		datalen = PACKET_SIZE - CAMERA_SIZ_STREAMHD;
		// 读出发送数据
		myMemcpy(sbuf + sendsize, payload, datalen);
		
		sendsize = datalen;
		datalen += CAMERA_SIZ_STREAMHD;
	} else{
		// 图像的后续包
		datalen = PACKET_SIZE - CAMERA_SIZ_STREAMHD;
		// 判断是否为最后一个数据包
		if (sendsize + datalen >= SBUF_SIZE)
		{
			datalen = SBUF_SIZE - sendsize;
			// 结束包标记(EOF置位,帧结束位指示视频结束，仅在属于图像帧的最后一个USB传输操作中设置该位)
			sendbuf[1] |= 0x02;
		}
		// 读出发送数据
		myMemcpy(sbuf + sendsize, payload, datalen);
		
		sendsize += datalen;
		datalen += CAMERA_SIZ_STREAMHD;
	}

	// 复制数据到PMA
	if (_GetENDPOINT(ENDP1) & EP_DTOG_TX)
	{
		
		// User use buffer0
		UserToPMABufferCopy(sendbuf, ENDP1_BUF0Addr, datalen);
		SetEPDblBuf0Count(ENDP1, EP_DBUF_IN, datalen);
	} else{
		// User use buffer1
		UserToPMABufferCopy(sendbuf, ENDP1_BUF1Addr, datalen);
		SetEPDblBuf1Count(ENDP1, EP_DBUF_IN, datalen);
	}
	_ToggleDTOG_TX(ENDP1);					// 反转DTOG_TX
	_SetEPTxStatus(ENDP1, EP_TX_VALID);		// 允许数据发送

	// 判断本帧图像是否发送完成
	if (sendsize >= SBUF_SIZE)
	{ 
		sendsize = 0; 
	}

	return;
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
	u32 Device_Serial0, Device_Serial1, Device_Serial2;

	Device_Serial0 = *(u32*)(0x1FFFF7E8);
	Device_Serial1 = *(u32*)(0x1FFFF7EC);
	Device_Serial2 = *(u32*)(0x1FFFF7F0);

	if (Device_Serial0 != 0)
	{
		Camera_StringSerial[2] = (u8)(Device_Serial0 & 0x000000FF);
		Camera_StringSerial[4] = (u8)((Device_Serial0 & 0x0000FF00) >> 8);
		Camera_StringSerial[6] = (u8)((Device_Serial0 & 0x00FF0000) >> 16);
		Camera_StringSerial[8] = (u8)((Device_Serial0 & 0xFF000000) >> 24);

		Camera_StringSerial[10] = (u8)(Device_Serial1 & 0x000000FF);
		Camera_StringSerial[12] = (u8)((Device_Serial1 & 0x0000FF00) >> 8);
		Camera_StringSerial[14] = (u8)((Device_Serial1 & 0x00FF0000) >> 16);
		Camera_StringSerial[16] = (u8)((Device_Serial1 & 0xFF000000) >> 24);

		Camera_StringSerial[18] = (u8)(Device_Serial2 & 0x000000FF);
		Camera_StringSerial[20] = (u8)((Device_Serial2 & 0x0000FF00) >> 8);
		Camera_StringSerial[22] = (u8)((Device_Serial2 & 0x00FF0000) >> 16);
		Camera_StringSerial[24] = (u8)((Device_Serial2 & 0xFF000000) >> 24);
	}
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

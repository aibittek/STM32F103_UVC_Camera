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


//����USBʱ��,USBclk=48Mhz
void Set_USBClock(void)
{
	RCC->CFGR &= ~(1 << 22); //USBclk=PLLclk/1.5=48Mhz	    
	RCC->APB1ENR |= 1 << 23; //USBʱ��ʹ��					 
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


// ���Բ���
#include "srcjpg.h"
//UVC payload head

#define CAMERA_SIZ_STREAMHD			2
u8 sendbuf[PACKET_SIZE] = { 0x02, 0x01 };			// �������ݻ�����
u32 sendsize = 0;									// �ѷ����ֽ���

void myMemcpy(const u8* src, u8* dst, u32 len)
{
	u32 i = 0;
	for (i = 0; i < len; ++i)
	{
		dst[i] = src[i];
	}
}

//USB�ж�����
void USB_Interrupts_Config(void)
{

	EXTI->IMR |= 1 << 18;//  ������18�ϵ��ж�
	EXTI->RTSR |= 1 << 18;//line 18���¼��������ش���	 
	MY_NVIC_Init(1, 0, USB_LP_CAN1_RX0_IRQn, 2);//��2�����ȼ���֮ 
	MY_NVIC_Init(0, 0, USBWakeUp_IRQn, 2);     //��2�����ȼ����	 	 
}

///*******************************************************************************
//* Function Name : UsbCamera_Send.
//* Description   : ��ʼ��������
//* Input         : 
//* Output        : 
//* Return value  : 
//*******************************************************************************/
//void UsbCamera_StreamStart(void)
//{
//	//u32 datalen = 0;
//
//	//��ʼ��UVC payload head
//	sendbuf[0] = 0x02;		//HLFΪ2(��ͷ�����ֶ�ָ���˱�ͷ�ĳ��ȣ���λΪ�ֽ�)
//	sendbuf[1] = 0x01;	
//	// ��ʼ����֡���ݴ������
//	sendsize = 0;
//
//	//// ���㷢�����ݳ���
//	//datalen = PACKET_SIZE - 2;
//	//myMemcpy(sbuf, sendbuf + 2, datalen);
//	//UserToPMABufferCopy(sendbuf, ENDP1_BUF0Addr, PACKET_SIZE);
//	//_SetEPDblBuf0Count(ENDP1, EP_DBUF_IN, PACKET_SIZE);
//	//sendsize = datalen;
//
//	// �յ�IN���ƺ�������
//	_SetEPTxStatus(ENDP1, EP_TX_VALID);
//}

/*******************************************************************************
* Function Name  : UsbCamera_Fillbuf
* Description    : ׼�������͵���Ƶ��������
* Input          :
* Output         :
* Return         :
*******************************************************************************/
void UsbCamera_Fillbuf(void)
{
	s32 datalen = 0;		// ���η��͵��ֽ���
	u8 *payload = 0;		// ��������ָ��

	// ���ͻ�������Ч���ݵ�ַ
	payload = sendbuf + CAMERA_SIZ_STREAMHD;
	// �����ݵ����ͻ�����
	if (0 == sendsize)
	{
		sendbuf[1] &= 0x01;		// ���BFH
		sendbuf[1] ^= 0x01;		// �л�FID

		// ���㱾�η������ݳ���
		datalen = PACKET_SIZE - CAMERA_SIZ_STREAMHD;
		// ������������
		myMemcpy(sbuf + sendsize, payload, datalen);
		
		sendsize = datalen;
		datalen += CAMERA_SIZ_STREAMHD;
	} else{
		// ͼ��ĺ�����
		datalen = PACKET_SIZE - CAMERA_SIZ_STREAMHD;
		// �ж��Ƿ�Ϊ���һ�����ݰ�
		if (sendsize + datalen >= SBUF_SIZE)
		{
			datalen = SBUF_SIZE - sendsize;
			// ���������(EOF��λ,֡����λָʾ��Ƶ��������������ͼ��֡�����һ��USB������������ø�λ)
			sendbuf[1] |= 0x02;
		}
		// ������������
		myMemcpy(sbuf + sendsize, payload, datalen);
		
		sendsize += datalen;
		datalen += CAMERA_SIZ_STREAMHD;
	}

	// �������ݵ�PMA
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
	_ToggleDTOG_TX(ENDP1);					// ��תDTOG_TX
	_SetEPTxStatus(ENDP1, EP_TX_VALID);		// �������ݷ���

	// �жϱ�֡ͼ���Ƿ������
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

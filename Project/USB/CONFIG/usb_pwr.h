/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_pwr.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Connection/disconnection & power management header
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PWR_H
#define __USB_PWR_H
#include "usb_type.h"
#include "usb_core.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _RESUME_STATE
{
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
} RESUME_STATE;


//1������̬��Attached����ȫ / �����豸D + �������1.5k�������裬�����豸D - �������1.5k�������裬�豸��������������ͨ������ź����ϵĵ�ƽ�仯�������豸�Ľ��룬����ȡ�豸�ٶȣ�
//2������̬��Powered�������Ǹ��豸���磬��Ϊ�豸����ʱ��Ĭ�Ϲ���ֵ�����ý׶κ�Ĺ���ֵ����������Ҫ������ֵ����ͨ��������ã�
//3��ȱʡ̬��Default����USB�ڱ�����֮ǰ��ͨ��ȱʡ��ַ0����������ͨ�ţ�
//4����ַ̬��Address�������������ã�USB�豸����λ�󣬾Ϳ��԰��������������Ψһ��ַ��������ͨ�ţ�����״̬���ǵ�ַ̬��
//5������̬��Configured����ͨ�����ֱ�׼��USB������������ȡ�豸�ĸ�����Ϣ�������豸��ĳ����Ϣ���иı�����á�
//6������̬��Suspended�������߹����豸��3ms��û�����߶�������USB���ߴ��ڿ���״̬�Ļ������豸��Ҫ�Զ��������״̬���ڽ������״̬���ܵĵ������Ĳ�����280UA��
typedef enum _DEVICE_STATE
{
	UNCONNECTED,	// 
	ATTACHED,		// ����̬
	POWERED,		// ����̬
	SUSPENDED,		// ����̬
	ADDRESSED,		// ��ַ̬
	CONFIGURED		// ����̬
} DEVICE_STATE;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Suspend (void);
void Resume_Init (void);
void Resume (RESUME_STATE eResumeSetVal);
RESULT PowerOn (void);
RESULT PowerOff (void);
/* External variables --------------------------------------------------------*/
extern  vu32 bDeviceState; /* USB device status */
extern volatile u8 fSuspendEnabled;  /* true when suspend is possible */

#endif  /*__USB_PWR_H*/

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

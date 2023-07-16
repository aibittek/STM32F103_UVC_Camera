#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "beep.h"	 	 
#include "key.h"	 	 
#include "exti.h"	 	 
#include "timer.h"		 	 
#include "tpad.h"	 	 
#include "lcd.h"
#include "usmart.h"	
#include "rtc.h"	 	 
#include "wkup.h"		 
#include "dac.h" 	  
#include "24cxx.h" 	 
#include "touch.h" 	 	
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	 
#include "usb_desc.h"



// ����USB ����/����
// enable:0,�Ͽ�
//        1,��������	   
void usb_port_set(u8 enable)
{
	RCC->APB2ENR |= 1 << 2;    				// ʹ��PORTAʱ��	   	 
	if (enable) 							// �˳��ϵ�ģʽ(0 == PDWN)
	{
		_SetCNTR(_GetCNTR()&(~(1 << 1))); 	// �˳��ϵ�ģʽ
	} else {		// ����ϵ�ģʽ(1 == PDWN)
		_SetCNTR(_GetCNTR() | (1 << 1));  	// �ϵ�ģʽ
		GPIOA->CRH &= 0XFFF00FFF;			// ���PA11��PA12
		GPIOA->CRH |= 0X00033000;			// 00��ͨ���������ģʽ ; 11 �����ģʽ������ٶ�50MH
		PAout(12) = 0;
	}
}

//u8 uvc_rdbuf[176] = {1};          //USB TX Buffer 
int main(void)
{
	Stm32_Clock_Init(9);	// ϵͳʱ������

	// USB����
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();

	while (1)
	{
	}
}

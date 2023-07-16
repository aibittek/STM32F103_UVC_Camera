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



// 设置USB 连接/断线
// enable:0,断开
//        1,允许连接	   
void usb_port_set(u8 enable)
{
	RCC->APB2ENR |= 1 << 2;    				// 使能PORTA时钟	   	 
	if (enable) 							// 退出断电模式(0 == PDWN)
	{
		_SetCNTR(_GetCNTR()&(~(1 << 1))); 	// 退出断电模式
	} else {		// 进入断电模式(1 == PDWN)
		_SetCNTR(_GetCNTR() | (1 << 1));  	// 断电模式
		GPIOA->CRH &= 0XFFF00FFF;			// 清除PA11和PA12
		GPIOA->CRH |= 0X00033000;			// 00：通用推挽输出模式 ; 11 ：输出模式，最大速度50MH
		PAout(12) = 0;
	}
}

//u8 uvc_rdbuf[176] = {1};          //USB TX Buffer 
int main(void)
{
	Stm32_Clock_Init(9);	// 系统时钟设置

	// USB配置
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();

	while (1)
	{
	}
}

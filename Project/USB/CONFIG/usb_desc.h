/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_desc.h
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Descriptor Header for Virtual COM Port Device
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define   UNCOMPRESS        0

#define MAKE_WORD(x)        (u8)((x)&0xFF),(u8)(((x)>>8)&0xFF)
#define MAKE_DWORD(x)       (u8)((x)&0xFF),(u8)(((x)>>8)&0xFF),(u8)(((x)>>16)&0xFF),(u8)(((x)>>24)&0xFF)


/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define USB_ASSOCIATION_DESCRIPTOR_TYPE         0x0B

#define IMG_WIDTH								640
#define IMG_HEIGHT								480
#define IMG_VIDEO_SCALE         2           //MJPEG size=Img_size/scale
//uvc-MJPEG帧率
#define IMG_MJPG_FRAMERATE      5           //预定义MJPEG视频帧率

#define PACKET_SIZE                             0xB0        //176 
#define MIN_BIT_RATE                        (20*1024*IMG_MJPG_FRAMERATE)
#define MAX_BIT_RATE                        (400*1024*IMG_MJPG_FRAMERATE)

#define MAX_FRAME_SIZE          (200*1024)      //最大每帧JPEG Byte数，对应Host要求的Buffer Size

#define FRAME_INTERVEL          (10000000ul/IMG_MJPG_FRAMERATE)     //帧间间隔时间，单位100ns

#ifdef UVC_1_1
#define CAMERA_SIZ_CONFIG_DESC                  144
#else
#define CAMERA_SIZ_CONFIG_DESC					192
#endif

#define CAMERA_SIZ_DEVICE_DESC                  18
#define CAMERA_SIZ_STRING_LANGID                4
#define CAMERA_SIZ_STRING_VENDOR                14
#define CAMERA_SIZ_STRING_PRODUCT               50
#define CAMERA_SIZ_STRING_SERIAL                26

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

/* Exported functions ------------------------------------------------------- */
extern const u8 Camera_DeviceDescriptor[CAMERA_SIZ_DEVICE_DESC];
extern const u8 Camera_ConfigDescriptor[CAMERA_SIZ_CONFIG_DESC];
extern const u8 Camera_StringLangID[CAMERA_SIZ_STRING_LANGID];
extern const u8 Camera_StringVendor[CAMERA_SIZ_STRING_VENDOR];
extern const u8 Camera_StringProduct[CAMERA_SIZ_STRING_PRODUCT];
extern u8 Camera_StringSerial[CAMERA_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

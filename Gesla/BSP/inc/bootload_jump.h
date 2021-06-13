#ifndef __BOOTLOAD_JUMP_H
#define __BOOTLOAD_JUMP_H

#include "stm32f10x.h"


#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define NEED_OTA_FLAG_ADDR ((uint32_t)0x08003002)

extern uint16_t NEED_OTA_FLAG;//1��Ҫ����FW  0����Ҫ����FW

void iap_load_app(u32 appxaddr);

#endif

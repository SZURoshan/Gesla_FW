#ifndef __BOOTLOAD_JUMP_H
#define __BOOTLOAD_JUMP_H

#include "stm32f10x.h"


#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址

void iap_load_app(u32 appxaddr);

#endif

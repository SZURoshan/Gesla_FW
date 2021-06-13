#include "bootload_jump.h"
#include <stdio.h>


__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

u16 Flash_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

uint16_t NEED_OTA_FLAG = 0;

//出现 FLASH_ERROR_PG 时，换用这种写法
void Writeflash(u32 addr,u16 data)
{
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASHStatus = FLASH_ErasePage(addr);

	if(FLASHStatus == FLASH_COMPLETE)
	{
		FLASH_ProgramHalfWord(addr,data);
	}
	FLASH_Lock();
}

typedef  void (*iapfun)(void);
iapfun jump2app; 
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查OTA bin里面的栈顶地址是否合法.
	{ 
		FLASH_Unlock();//解锁
		Writeflash(NEED_OTA_FLAG_ADDR, 1);//flash 重置
		FLASH_Lock();//上锁
		
		
		NEED_OTA_FLAG = Flash_ReadHalfWord(NEED_OTA_FLAG_ADDR);
		if(NEED_OTA_FLAG == 1)
		{
			printf("FW set NEED_OTA_FLAG: %d \r\n", NEED_OTA_FLAG);
		}
		else
		{
			printf("FW set NEED_OTA_FLAG fail... \r\n");
		}
		
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
	else
	{
		printf("illegal ota bin MSP: %x \r\n", ((*(vu32*)appxaddr)&0x2FFE0000) );
	}
}

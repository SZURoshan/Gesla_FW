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

//���� FLASH_ERROR_PG ʱ����������д��
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
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���OTA bin�����ջ����ַ�Ƿ�Ϸ�.
	{ 
		FLASH_Unlock();//����
		Writeflash(NEED_OTA_FLAG_ADDR, 1);//flash ����
		FLASH_Lock();//����
		
		
		NEED_OTA_FLAG = Flash_ReadHalfWord(NEED_OTA_FLAG_ADDR);
		if(NEED_OTA_FLAG == 1)
		{
			printf("FW set NEED_OTA_FLAG: %d \r\n", NEED_OTA_FLAG);
		}
		else
		{
			printf("FW set NEED_OTA_FLAG fail... \r\n");
		}
		
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
	else
	{
		printf("illegal ota bin MSP: %x \r\n", ((*(vu32*)appxaddr)&0x2FFE0000) );
	}
}

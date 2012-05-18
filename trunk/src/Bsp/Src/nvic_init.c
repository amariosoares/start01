#include <nvic_init.h>

/****************************************************************************
* ��	�ƣ�void nvic_init(void)
* ��	�ܣ��жϹ����ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵	������
****************************************************************************/
void nvic_init(void)
{
	#ifdef  VECT_TAB_RAM	//�ж��������ŵ�ַ��SRAM 0x20000000  
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else					//�ж��������ŵ�ַ��FLASH	0x08000000
  	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); 
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xA000);   
  
	#endif
}

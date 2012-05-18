#ifndef	_IOPORT_H_
#define _IOPORT_H_

#ifdef __cplusplus		   //�����CPP����C���� //��ʼ����
extern "C" {
#endif

//private=====================================================================

#define PORT(x)		GPIO##x				   //IO�˿�
#define BIT(x)		(1<<x)				   //IO����

//public======================================================================
/////////////////////////////IO�˿�����/////////////////////////////////
#define GPIO_ANALOG_INPUT			0
#define GPIO_FLOATING_INPUT			4
#define GPIO_PULL_UP_DOWN_INPUT		8
#define GPIO_PUSH_PULL_10M_OUTPUT	1
#define GPIO_PUSH_PULL_2M_OUTPUT	2
#define GPIO_PUSH_PULL_50M_OUTPUT	3
#define GPIO_OPEN_DRAIN_10M_OUTPUT	5
#define GPIO_OPEN_DRAIN_2M_OUTPUT	6
#define GPIO_OPEN_DRAIN_50M_OUTPUT	7
#define AFIO_PUSH_PULL_10M_OUTPUT	9
#define AFIO_PUSH_PULL_2M_OUTPUT	10
#define AFIO_PUSH_PULL_50M_OUTPUT	11
#define AFIO_OPEN_DRAIN_10M_OUTPUT	13
#define AFIO_OPEN_DRAIN_2M_OUTPUT	14
#define AFIO_OPEN_DRAIN_50M_OUTPUT	15

///////////////////////////����IO�˿�����/////////////////////////////////
#define SET_IO_ATTRI(port, bit, attri)  	(*(&(port->CRL) + ((bit)>>3))) &= ~(0x0FUL << 4*(bit&0x07)); \
											(*(&(port->CRL) + ((bit)>>3))) |= (attri << 4*(bit&0x07))	 //����IO����


#define IoPushPull(bit)				SET_IO_ATTRI(bit##_PORT, bit, GPIO_PUSH_PULL_50M_OUTPUT)	 //���ùܽ��������
#define IoOpenDrain(bit)			SET_IO_ATTRI(bit##_PORT, bit, GPIO_OPEN_DRAIN_50M_OUTPUT)	 //���ùܽſ�©���
#define IoInFloating(bit)			SET_IO_ATTRI(bit##_PORT, bit, GPIO_FLOATING_INPUT)			 //���ù̸ܽ�������

#define IoHi(bit)					(bit##_PORT##->ODR) |= 1<<bit 					   //������1
#define IoLo(bit)					(bit##_PORT##->ODR) &= ~(1<<bit)				   //������0

#define GetIo(bit)				((bit##_PORT##->IDR & (1<<bit)) >> bit)	   			//��ȡ���ŵ�ƽ

#define DataInitZero(x) 			memset(x, 0, sizeof(x))			//��������x�����ݿռ��ʼ��Ϊ0
#define DataInit(x, nData)			memset(x, nData, sizeof(x))		//��������x�����ݸ�ֵΪnData
		
//************************************************************************
#endif

#ifdef __cplusplus		   //�����CPP����C���� //��������
}
#endif

/*
#define X 1						//X����һ�� 1���ź�
#define X_PORT	GPIOB		   	//�˿ں����_PORT

MakePushPull(X);				//���������ó�Ϊ�������
MakeOpenDrain(X);				//���������ó�Ϊ��©���
SetHi(X);						//��1
SetLo(X);						//��0
bool GetSignal(X);				//��ȡ���ŵ�ƽ
*/

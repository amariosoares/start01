#ifndef __PROTOCAL_H
#define __PROTOCAL_H

typedef enum {
	CMD_READ_INT1=0,
	CMD_READ_INT2,
	CMD_READ_INT3,
	CMD_READ_INT4,
	CMD_READ_FLOAT1,
	CMD_READ_FLOAT2,
	CMD_READ_FLOAT3,
	CMD_READ_FLOAT4,
	CMD_READ_DATE,
    	CMD_READ_TIME ,		//�豸��ʱ�� 	
	CMD_LOCAL_IP,		
	CMD_LOCAL_PORT,
	CMD_GATEWAY,
	CMD_NETMASK,
	CMD_MAC,
	CMD_DEV_ADDR ,		//�豸�ĵ�ַ
	CMD_CPU_MODE,	//���ӳ�̨
	CMD_REPORT_STATE, //�����豸״̬
	CMD_RESET_CNT, //ϵͳ��λ����
	CMD_MEM_DEBUG, //��̬�ڴ������Ϣ����ʾ��ǰ��̬�ڴ�ʹ����
	CMD_NET_SEND_AD, //ͨ�����緢�Ͳ���
	CMD_UDP_IP,
	CMD_UDP_PORT,
	CMD_MAX,
}TCmdType;





#endif


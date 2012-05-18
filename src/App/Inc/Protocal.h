#ifndef __PROTOCAL_H
#define __PROTOCAL_H

typedef enum {
	CMD_K =0, //ϵͳk ϵ��
	CMD_DATETIME ,		//�豸��ʱ��
	CMD_DEV_ADDR ,		//�豸�ĵ�ַ
	CMD_SEND_INTERVAL,	//���ݷ��ͼ��
	CMD_UP1,	//��1·�ϳ�̨��ֵ
	CMD_UP2,   // �ڶ�·�ϳ�̨��ֵ
	CMD_DOWN1,   //AD1�³�̨
	CMD_DOWN2,   //AD2�³�̨
	CMD_ZERO1,	//��һ·���
	CMD_ZERO2,	//�ڶ�·���
	CMD_UP,  //�ϳ�̨��ֵ
	CMD_DOWN, //�³�̨��ֵ
	CMD_ZERO,  //ϵͳ���
	CMD_READ_PAGE_ADDR,
	CMD_WRITE_PAGE_ADDR,
	CMD_READ_PAGE_OFFSET,
	CMD_WRITE_PAGE_OFFSET,
	CMD_WRITE_VEH_DATA,		//д��һ��ģ���������ݵ��洢�豸
	CMD_SEND_AD,		//�Զ�����AD ֵ
	CMD_EN_ZERO_TRACE,	//����������
	CMD_DIR1,
	CMD_DIR2,			
	CMD_MIN_VALID_AD_CNT,
	CMD_SCALER_LEN,	//��̨��� mm
	CMD_LIMIT_KG,	//�ϳ�̨��ֵ
	CMD_FIRST_AD_FILTERCNT,	//��1·�˲�ϵ��
	CMD_SECOND_AD_FILTERCNT, //��2·�˲�ϵ��
	CMD_SYNC_ZERO,		//ͬ�����
	CMD_CPU_MODE,	//���ӳ�̨
	CMD_GM_OPEN, 	//ģ�����ù�Ļ״̬
	CMD_REPORT_STATE, //�����豸״̬
	CMD_GET_AD,	//��ȡAD
	CMD_DIV, //�ֶ�ֵ
	CMD_DOTNUM, //С������ʾλ��
	CMD_BUS,  //�Ƿ�����ó�׳�
	CMD_ZEROSPD	, //�������ٶ�
	CMD_ZEROSTOP,	 //������ֹͣ��Χ
	CMD_PWRSPAN,	//�������㷶Χ
	CMD_HDSPAN,   //�ֶ����㷶Χ
	CMD_K1,		// k1
	CMD_K2,		// k2	
	CMD_SK,		//static k
	CMD_DISCARD_TARE, //ȥƤ
	CMD_CHANGE_STATIC, //����̬ģʽ�л�
	CMD_WEIGHT, //��������
	CMD_HEART, //����������
	CMD_RESET_CNT, //ϵͳ��λ����
	CMD_MEM_DEBUG, //��̬�ڴ������Ϣ����ʾ��ǰ��̬�ڴ�ʹ����
	CMD_STATIC_WEIGHT, //��ȡĳ����̨�ľ�̬����
	CMD_NET_SEND_AD, //ͨ�����緢�Ͳ���
	CMD_LOCAL_IP,		
	CMD_LOCAL_PORT,
	CMD_UDP_IP,
	CMD_UDP_PORT,
	CMD_MAX,
}TCmdType;





#endif


#ifndef __PROTOCAL_H
#define __PROTOCAL_H

typedef enum {
	CMD_K =0, //系统k 系数
	CMD_DATETIME ,		//设备的时间
	CMD_DEV_ADDR ,		//设备的地址
	CMD_SEND_INTERVAL,	//数据发送间隔
	CMD_UP1,	//第1路上秤台阀值
	CMD_UP2,   // 第二路上秤台阀值
	CMD_DOWN1,   //AD1下称台
	CMD_DOWN2,   //AD2下称台
	CMD_ZERO1,	//第一路零点
	CMD_ZERO2,	//第二路零点
	CMD_UP,  //上秤台阀值
	CMD_DOWN, //下秤台阀值
	CMD_ZERO,  //系统零点
	CMD_READ_PAGE_ADDR,
	CMD_WRITE_PAGE_ADDR,
	CMD_READ_PAGE_OFFSET,
	CMD_WRITE_PAGE_OFFSET,
	CMD_WRITE_VEH_DATA,		//写入一条模拟重量数据到存储设备
	CMD_SEND_AD,		//自动发送AD 值
	CMD_EN_ZERO_TRACE,	//运行零点跟踪
	CMD_DIR1,
	CMD_DIR2,			
	CMD_MIN_VALID_AD_CNT,
	CMD_SCALER_LEN,	//秤台宽度 mm
	CMD_LIMIT_KG,	//上秤台阀值
	CMD_FIRST_AD_FILTERCNT,	//第1路滤波系数
	CMD_SECOND_AD_FILTERCNT, //第2路滤波系数
	CMD_SYNC_ZERO,		//同步零点
	CMD_CPU_MODE,	//主从秤台
	CMD_GM_OPEN, 	//模拟设置光幕状态
	CMD_REPORT_STATE, //报告设备状态
	CMD_GET_AD,	//读取AD
	CMD_DIV, //分度值
	CMD_DOTNUM, //小数点显示位数
	CMD_BUS,  //是否用于贸易秤
	CMD_ZEROSPD	, //零点跟踪速度
	CMD_ZEROSTOP,	 //零点跟踪停止范围
	CMD_PWRSPAN,	//开机置零范围
	CMD_HDSPAN,   //手动置零范围
	CMD_K1,		// k1
	CMD_K2,		// k2	
	CMD_SK,		//static k
	CMD_DISCARD_TARE, //去皮
	CMD_CHANGE_STATIC, //动静态模式切换
	CMD_WEIGHT, //重量数据
	CMD_HEART, //心跳包数据
	CMD_RESET_CNT, //系统复位次数
	CMD_MEM_DEBUG, //动态内存调试信息，显示当前动态内存使用量
	CMD_STATIC_WEIGHT, //获取某个秤台的静态重量
	CMD_NET_SEND_AD, //通过网络发送波形
	CMD_LOCAL_IP,		
	CMD_LOCAL_PORT,
	CMD_UDP_IP,
	CMD_UDP_PORT,
	CMD_MAX,
}TCmdType;





#endif


#include "includes.h"
#include "datetime.h"

#define START_YEAR		2000


static RTC_Device* rtc_dev = NULL;

#define SecsPerComYear  31536000	//(365*3600*24)
#define SecsPerLeapYear 31622400	//(366*3600*24)
#define SecsPerFourYear 126230400	//((365*3600*24)*3+(366*3600*24))
#define SecsPerDay      86400		//(3600*24)


//从闰年开始到下一个闰年,每年所经过的秒数
s32 Year_Secs_Accu[5]={0,
                      31622400,
                      63158400,
                      94694400,
                      126230400};

//平年每个月所经过的秒数
s32 Month_Secs_Accu_C[13] = { 0,
                            2678400,
                            5097600,
                            7776000,
                            10368000,
                            13046400,
                            15638400,
                            18316800,
                            20995200,
                            23587200,
                            26265600,
                            28857600,
                            31536000};

//闰年每个月所经过的秒数
s32 Month_Secs_Accu_L[13] = {0,
                            2678400,
                            5184000,
                            7862400,  
                            10454400,
                            13132800,
                            15724800,
                            18403200,
                            21081600,
                            23673600,
                            26352000,
                            28944000,
                            31622400};


static int mdays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 }; 
static int Ymd2Wday(int year, int month, int days) //年月日 to 星期
{ 
	int i, y = year - 1; 
	for (i=0; i<month; ++i) 
	{
		days += mdays[i];
	}

	if (month>2) 
	{ 
		// 如果是闰年,则在2月后加上一天
		if (((year%400)==0) || ((year&3)==0 && (year%100))) ++days; 
	} 

	return ((y+y/4-y/100+y/400+days)%7); 
} 

void datetime_convert(u32 TimeVar,TDateTime* dt)
{
	s32 Num4Y,NumY, OffSec, Off4Y = 0;
	u32 i;

	s32 NumDay;

	Num4Y  = TimeVar/SecsPerFourYear;
	OffSec = TimeVar%SecsPerFourYear;

	i=1;
	while(OffSec > Year_Secs_Accu[i++])
		Off4Y++;

	/* 经过的年数 */
	NumY = Num4Y*4 + Off4Y;
	/* 当前年份*/
	dt->year = START_YEAR+NumY;

	OffSec = OffSec - Year_Secs_Accu[i-2];

	/* 算月份*/
	i=0;
	if(dt->year % 4)
	{// 平年
		while(OffSec > Month_Secs_Accu_C[i++]);
		dt->mon = i-1;
		OffSec = OffSec - Month_Secs_Accu_C[i-2];
	}
	else
	{// 闰年
		while(OffSec > Month_Secs_Accu_L[i++]);
		dt->mon = i-1;
		OffSec = OffSec - Month_Secs_Accu_L[i-2];
	}

	/* 算一个月中的天数 */
	NumDay = OffSec/SecsPerDay;
	OffSec = OffSec%SecsPerDay;
	dt->day = NumDay+1;

	/* 算小时 */
	dt->hour = OffSec/3600;
	/* 算分钟 */
	dt->min = (OffSec % 3600)/60;
	/* 算秒数 */
	dt->sec = (OffSec % 3600)% 60;
	/* 算星期 */
	dt->week = Ymd2Wday(dt->year, dt->mon, dt->day);

}

/**************************************************
**函数原型:	static inline unsigned long mktime (unsigned int year, unsigned int mon, 
**												unsigned int day, unsigned int hour, 
**												unsigned int min, unsigned int sec)
**功能:		计算用户设置的时间, 以秒为单位
**输入参数:	year 年 4位整数
**			mon  月 2位整数
**			day  日 2位整数
**			hour 时 2位整数
**			min  分 2位整数
**			sec  秒 2位整数
**返回参数:	返回设置 RTC 计数器的值
**************************************************/
unsigned long datetime_mktime (TDateTime* dt)
{ 
	if(dt==NULL) return 0;
	if (0 >= (int)(dt->mon -= 2))
	{    
		dt->mon += 12;      
		dt->year -= 1; 
	} 

	return (((( 
		(unsigned long) (dt->year/4 - dt->year/100 + dt->year/400 + 367*dt->mon/12 + dt->day) + 
		dt->year*365 - 730456)* 	//719499
		24 + dt->hour )*	/**//* now have hours */ 
		60 + dt->min )*		/**//* now have minutes */ 
		60 + dt->sec);		/**//* finally seconds */ 
}


bool get_zipdatetime(TZipDateTimeDef* zdt)
{
	TDateTime dt;
	if(get_datetime(&dt) && zdt){
		zdt->value = datetime_zip(&dt);
		return true;
	}
	return false;
}
bool set_zipdatetime(TZipDateTimeDef* zdt)
{
	TDateTime dt;

	if(datetime_unzip(zdt->value,&dt))
	{
		return set_datetime(&dt);
	}
	return false;
}


bool get_datetime(TDateTime* dt)
{
	if(rtc_dev){
		return rtc_dev->get_datetime(dt);
	}
	return false;
}
bool set_datetime(TDateTime* dt)
{
	if(rtc_dev){
		return rtc_dev->set_datetime(dt);
	}
	return false;
}
u16  get_year()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.year;
	}
	return 0;
}
u8   get_mon()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.mon;
	}
	return 0;
}
u8   get_day()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.day;
	}
	return 0;
}
u8   get_hour()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.hour;
	}
	return 0;
}
u8	 get_min()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.min;
	}
	return 0;
}
u8   get_sec()
{
	TDateTime dt;
	if(rtc_dev){
		if(rtc_dev->set_datetime(&dt))
			return dt.sec;
	}
	return 0;
}
bool valid_datetime(TDateTime* dt)
{
	if(dt){
		if(dt->year < 2012 || dt->year > 2022) return false;
		if(dt->mon <  1 || dt->mon > 12) return false;
		if(dt->day > 31) return false;
		if(dt->hour > 24) return false;
		if(dt->min > 60) return false;
		if(dt->sec > 59) return false;
		
		return true;
	}
	return false;
}

bool datetime_unzip(unsigned int zip_value,TDateTime *dt)
{
	TZipDateTimeDef zdt;
	zdt.value = zip_value;

	if(dt){
		dt->year = zdt.dt.year+2000;
		dt->mon  = zdt.dt.mon;
		dt->day  = zdt.dt.day;
		dt->hour = zdt.dt.hour;
		dt->min  = zdt.dt.min;
		dt->sec  = zdt.dt.sec;
		
		return true;
	}

	return false;
}
/*
返回压缩后的值
*/
unsigned int datetime_zip(TDateTime* in_dt)
{
	TZipDateTimeDef out_zdt;
	if(in_dt){
		out_zdt.dt.year = (in_dt->year-2000)&0x3f;
		out_zdt.dt.mon  = in_dt->mon&0x0f;
		out_zdt.dt.day  = in_dt->day&0x1f;
		out_zdt.dt.hour = in_dt->hour&0x1f;
		out_zdt.dt.min  = in_dt->min&0x3f ;
		out_zdt.dt.sec  = in_dt->sec&0x3f;

		return out_zdt.value;
	}	
	return 0;
}

bool check_rtc_exist(void)
{
	TDateTime dt;
	
	if(get_datetime(&dt)){
		if(valid_datetime(&dt)) return true;
		dt.year = 2012;
		dt.mon  = 5;
		dt.day  = 10;
		dt.hour = 10;
		dt.min	= 12;
		dt.sec  = 12;
		set_datetime(&dt);
		memset((void*)&dt,0,sizeof(TDateTime));
		if(get_datetime(&dt)){
			if(dt.year != 2012 )return false;
			if(dt.mon != 5 ) 	return false;
			if(dt.day != 10 ) 	return false;
			if(dt.hour != 10 ) 	return false;
			if(dt.min != 12 ) 	return false;
			if(dt.sec != 12 ) 	return false;
			return true;
		}
	}

	return false;
	
}

bool register_rtc(RTC_Device* dev)
{
	rtc_dev = dev;
	
	return dev!=NULL;
}



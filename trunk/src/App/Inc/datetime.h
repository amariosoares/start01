#ifndef DATE_TIME_H
#define DATE_TIME_H
#define MAX_RTC_DEV_NAME 16
typedef struct{
	u16 year;
	u8  mon;
	u8  day;
	u8  week;
	u8  hour;
	u8  min;
	u8  sec;	
}TDateTime;

typedef struct{
	unsigned  sec:6;
	unsigned  min:6;
	unsigned  hour:5;
	unsigned  day:5;
	unsigned  mon:4;
	unsigned  year:6;
}TZipDateTime;

typedef union 
{
	TZipDateTime dt;
	unsigned     value;
}TZipDateTimeDef;

typedef bool (*Get_DateTime)(TDateTime* dt);
typedef bool (*Set_DateTime)(TDateTime* dt);
typedef struct{
	char name[MAX_RTC_DEV_NAME];
	Get_DateTime get_datetime;
	Set_DateTime set_datetime;
	
}RTC_Device;
bool get_datetime(TDateTime* dt);
bool set_datetime(TDateTime* dt);
bool valid_datetime(TDateTime* dt);
bool get_zipdatetime(TZipDateTimeDef* zdt);
bool set_zipdatetime(TZipDateTimeDef* zdt);


bool datetime_unzip(unsigned int zip_value,TDateTime *dt);
unsigned int datetime_zip(TDateTime* in_dt);
unsigned long datetime_mktime (TDateTime* dt);
void datetime_convert(u32 TimeVar,TDateTime* dt);

u16  get_year(void);
u8   get_mon(void);
u8   get_day(void);
u8   get_hour(void);
u8	 get_min(void);
u8   get_sec(void);
bool register_rtc(RTC_Device* dev);
#endif


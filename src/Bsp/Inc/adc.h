#ifndef ADC_H
#define ADC_H
typedef enum{
	GAIN1,
	GAIN2,
	GAIN4,
	GAIN8,
	GAIN16,
	GAIN32,
	GAIN64
}TGainFactor;

typedef enum{
	SPS6P25,
	SPS12P5,
	SPS25,
	GAIN8,
	GAIN16,
	GAIN32,
	GAIN64
}T50HZWordRate;
struct TADCDevice{
	
};
typedef enum{
	CS5530,
	CS5532,
	CS5534
}ADCType;
struct TADCDevice* request_adc();
BOOL adc_setgain(int gain);
BOOL adc_setspeed(double sps);
BOOL adc_reset();
BOOL adc_start();
BOOL adc_stop();
int  adc_read();

BOOL register_adc(TADCDevice* dev);
#endif

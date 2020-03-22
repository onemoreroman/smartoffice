/***************************************************
 * CCS811 library class v0.2 / 20190624
 * CCS811 TVOC/eCO2 sensors are manufactured by AMS
 *
 * This CCS811 CO2 and tVOC class - based on test software (Beerware license) provided by Nathan Seidle from SparkFun Electronics. 
 * Thank you Nathan! Great job! 
 * We've ported Nathan's functions into a class, add some variables, functions and functionalities.
 *
 * This library it's compatible with:
 *		s-Sense CCS811 I2C sensor breakout [PN: SS-CCS811#I2C, SKU: ITBP-6004], info https://itbrainpower.net/sensors/CCS811-CO2-TVOC-I2C-sensor-breakout 
 *		s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 * 
 * 
 * 
 * CCS811 definitions are placed bellow, in lines 38-40;
 *
 * New CCS811 sensors requires at 48-burn in. Once burned in a sensor requires 20 minutes of run in before readings are considered good.
 * READ CCS811 documentation! https://itbrainpower.net/downloadables/CCS811_DS000459_5-00.pdf
 * 
 * You are legaly entitled to use this SOFTWARE ONLY IN CONJUNCTION WITH s-Sense CCS811 I2C sensors DEVICES USAGE. Modifications, derivates and redistribution 
 * of this software must include unmodified this COPYRIGHT NOTICE. You can redistribute this SOFTWARE and/or modify it under the terms 
 * of this COPYRIGHT NOTICE. Any other usage may be permited only after written notice of Dragos Iosub / R&D Software Solutions srl.
 * 
 * This SOFTWARE is distributed is provide "AS IS" in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *  
 *  
 * itbrainpower.net invests significant time in design phase of our IoT products and in associated software and support resources.
 * Support us by purchasing our environmental and air quality sensors from here https://itbrainpower.net/order#s-Sense
 *
 *
 *
 * Dragos Iosub, Bucharest 2019.
 * https://itbrainpower.net
 */

 
#define I2C_CCS811_ADDRESS 0x5A         //CCS811 I2C address
#define CCS811_WAKE_PIN 5               //CCS811 WAKE PIN
//#define CCS811_INT_PIN 8              //CCS811 IRQ PIN

#ifndef CCS811_h
#define CCS811_h

#include "Arduino.h"

#ifdef  __SAMD21G18A__
  #define DebugPort SerialUSB
#else
  #define DebugPort Serial 
#endif

#include "Wire.h"   // Wire library for I2C protocol

//readMode
#define driveMode_Idle	0
#define driveMode_1sec	1
#define driveMode_10sec	2
#define driveMode_60sec	3
#define driveMode_RAW	4

//Register addresses
#define CSS811_STATUS 0x00
#define CSS811_MEAS_MODE 0x01
#define CSS811_ALG_RESULT_DATA 0x02
#define CSS811_RAW_DATA 0x03
#define CSS811_ENV_DATA 0x05
#define CSS811_NTC 0x06
#define CSS811_THRESHOLDS 0x10
#define CSS811_BASELINE 0x11
#define CSS811_HW_ID 0x20
#define CSS811_HW_VERSION 0x21
#define CSS811_FW_BOOT_VERSION 0x23
#define CSS811_FW_APP_VERSION 0x24
#define CSS811_ERROR_ID 0xE0
#define CSS811_APP_START 0xF4
#define CSS811_SW_RESET 0xFF

class CCS811
{
  public:
    CCS811();     // constructor
    boolean begin(uint8_t I2C_ADDR, uint8_t WAKE_PIN, int driveMode);
	
	void readAlgorithmResults(void);
	int configureCCS811(void);//do drive mode!!!
	boolean checkForError(void);
	void wakeup(void);
	void sleep(void);
	void printError(void);
	unsigned int getBaseline(void);
	boolean checkDataAndUpdate(void);
	boolean dataAvailable(void);
	boolean appValid(void);
	void enableInterrupts(void);
	void disableInterrupts(void);
	void setDriveMode(byte mode);
	void setEnvironmentalData(float relativeHumidity, float temperature);

	byte readRegister(byte addr);
	void writeRegister(byte addr, byte val);

	//int TVOC, CO2;
	unsigned int getCO2(void);
	unsigned int gettVOC(void);
	
	unsigned int tVOC, CO2;

  private:
    uint8_t _WAKE_PIN;
    uint8_t _I2C_ADDR;
	int _driveMode;
};

#endif

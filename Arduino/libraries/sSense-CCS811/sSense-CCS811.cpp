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
 * CCS811 definitions are placed in sSense-CCS811.h:
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

#include "sSense-CCS811.h"

CCS811::CCS811()
{
  // empty constructor, just because.
}


boolean CCS811::begin(uint8_t I2C_ADDR, uint8_t WAKE_PIN, int driveMode)// driveMode ITBP add
{
  delay(100); 
  _I2C_ADDR = I2C_ADDR; //ITBP add
  _WAKE_PIN = WAKE_PIN; //ITBP add
  _driveMode = driveMode; //ITBP add
  
  Wire.begin();
  wakeup();
  delay(500);   

  //configureCCS811(); //Turn on sensor
  if(configureCCS811()!=0){return false;}; //ITBP add check no errors at Turn on sensor

  unsigned int result = getBaseline();

  DebugPort.print("baseline for this sensor: 0x");
  if(result < 0x100) DebugPort.print("0");
  if(result < 0x10) DebugPort.print("0");
  DebugPort.println(result, HEX);
  
  return true;
}

void CCS811::wakeup()// wake up CCS811 ITBP add
{
  pinMode(_WAKE_PIN, OUTPUT);
  digitalWrite(_WAKE_PIN, LOW);	
}

void CCS811::sleep()// goto sleep CCS811 ITBP add
{
  pinMode(_WAKE_PIN, INPUT); //emulate three state mode + external pull-up ==> WAKE goes HIGH 
}

//Updates the total voltatile organic compounds (TVOC) in parts per billion (PPB)
//and the CO2 value
//Returns nothing
void CCS811::readAlgorithmResults()
{
  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR);
  Wire.write(CSS811_ALG_RESULT_DATA);
  Wire.endTransmission();

  //Wire.requestFrom(CCS811_ADDR, 4); //Get four bytes
  Wire.requestFrom(_I2C_ADDR, 4); //Get four bytes

  byte co2MSB = Wire.read();
  byte co2LSB = Wire.read();
  byte tvocMSB = Wire.read();
  byte tvocLSB = Wire.read();

  CO2 = ((unsigned int)co2MSB << 8) | co2LSB;
  tVOC = ((unsigned int)tvocMSB << 8) | tvocLSB;
}


//Turns on the sensor and configures it with default settings
//void CCS811::configureCCS811()//ITBP add 			changed function type from void to int
int CCS811::configureCCS811()//ITBP add 			changed function type from void to int
{
  //Verify the hardware ID is what we expect
  byte hwID = readRegister(0x20); //Hardware ID should be 0x81
  if (hwID != 0x81)
  {
    DebugPort.println("CCS811 not found. Please check wiring.");
    DebugPort.println(hwID, HEX);
	
	return -1; //ITBP add
    //while (1); //Freeze!
  }

  //Check for errors
  if (checkForError() == true)
  {
    DebugPort.println("Error at Startup");
    printError();

	return -2; //ITBP add
    //while (1); //Freeze!
  }

  //Tell App to Start
  if (appValid() == false)
  {
    DebugPort.println("Error: App not valid.");

	return -3; //ITBP add
    //while (1); //Freeze!
  }

  //Write to this register to start app
  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR);
  Wire.write(CSS811_APP_START);
  Wire.endTransmission();

  //Check for errors
  if (checkForError() == true)
  {
    DebugPort.println("Error at AppStart");
    printError();

	return -4; //ITBP add
    //while (1); //Freeze!
  }

  //Set Drive Mode
  //setDriveMode(1); //Read every second
  setDriveMode(_driveMode); // Mode 0/1/2/3/4==> Idle/1sec/10sec/60sec/RAW		ITBP add

  //Check for errors
  if (checkForError() == true)
  {
    DebugPort.println("Error at setDriveMode");
    printError();

	return -5; //ITBP add
    //while (1); //Freeze!
  }

  return 0; //ITBP add  
}

//Checks to see if error bit is set
boolean CCS811::checkForError()
{
  byte value = readRegister(CSS811_STATUS);
  return (value & 1 << 0);
}

//Displays the type of error
//Calling this causes reading the contents of the ERROR register
//This should clear the ERROR_ID register
void CCS811::printError()
{
  byte error = readRegister(CSS811_ERROR_ID);

  DebugPort.print("Error: ");
  if (error & 1 << 5) DebugPort.print("HeaterSupply ");
  if (error & 1 << 4) DebugPort.print("HeaterFault ");
  if (error & 1 << 3) DebugPort.print("MaxResistance ");
  if (error & 1 << 2) DebugPort.print("MeasModeInvalid ");
  if (error & 1 << 1) DebugPort.print("ReadRegInvalid ");
  if (error & 1 << 0) DebugPort.print("MsgInvalid ");

  DebugPort.println();
}

//Returns the baseline value
//Used for telling sensor what 'clean' air is
//You must put the sensor in clean air and record this value
unsigned int CCS811::getBaseline()
{
  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR);	//ITBP add
  Wire.write(CSS811_BASELINE);
  Wire.endTransmission();

  //Wire.requestFrom(CCS811_ADDR, 2); //Get two bytes
  Wire.requestFrom(_I2C_ADDR, 2); //ITBP add		Get two bytes

  byte baselineMSB = Wire.read();
  byte baselineLSB = Wire.read();

  unsigned int baseline = ((unsigned int)baselineMSB << 8) | baselineLSB;

  return (baseline);
}


boolean CCS811::checkDataAndUpdate()	//ITBP add
{
  if(dataAvailable())
  {
	readAlgorithmResults(); //update the global tVOC and CO2 variables
	return true;
  }
  return false;
}


//Checks to see if DATA_READ flag is set in the status register
boolean CCS811::dataAvailable()
{
  byte value = readRegister(CSS811_STATUS);
  return (value & 1 << 3);
}

//Checks to see if APP_VALID flag is set in the status register
boolean CCS811::appValid()
{
  byte value = readRegister(CSS811_STATUS);
  return (value & 1 << 4);
}

//Enable the nINT signal
void CCS811::enableInterrupts(void)
{
  byte setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
  setting |= (1 << 3); //Set INTERRUPT bit
  writeRegister(CSS811_MEAS_MODE, setting);
}

//Disable the nINT signal
void CCS811::disableInterrupts(void)
{
  byte setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
  setting &= ~(1 << 3); //Clear INTERRUPT bit
  writeRegister(CSS811_MEAS_MODE, setting);
}

//Mode 0 = Idle
//Mode 1 = read every 1s
//Mode 2 = every 10s
//Mode 3 = every 60s
//Mode 4 = RAW mode
void CCS811::setDriveMode(byte mode)
{
  if (mode > 4) mode = 4; //Error correction

  byte setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there

  setting &= ~(0b00000111 << 4); //Clear DRIVE_MODE bits
  setting |= (mode << 4); //Mask in mode
  writeRegister(CSS811_MEAS_MODE, setting);
}

//Given a temp and humidity, write this data to the CSS811 for better compensation
//This function expects the humidity and temp to come in as floats
void CCS811::setEnvironmentalData(float relativeHumidity, float temperature)
{
  int rH = relativeHumidity * 1000; //42.348 becomes 42348
  int temp = temperature * 1000; //23.2 becomes 23200

  byte envData[4];

  //Split value into 7-bit integer and 9-bit fractional
  envData[0] = ((rH % 1000) / 100) > 7 ? (rH / 1000 + 1) << 1 : (rH / 1000) << 1;
  envData[1] = 0; //CCS811 only supports increments of 0.5 so bits 7-0 will always be zero
  if (((rH % 1000) / 100) > 2 && (((rH % 1000) / 100) < 8))
  {
    envData[0] |= 1; //Set 9th bit of fractional to indicate 0.5%
  }

  temp += 25000; //Add the 25C offset
  //Split value into 7-bit integer and 9-bit fractional
  envData[2] = ((temp % 1000) / 100) > 7 ? (temp / 1000 + 1) << 1 : (temp / 1000) << 1;
  envData[3] = 0;
  if (((temp % 1000) / 100) > 2 && (((temp % 1000) / 100) < 8))
  {
    envData[2] |= 1;  //Set 9th bit of fractional to indicate 0.5C
  }

  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR); //ITBP add
  Wire.write(CSS811_ENV_DATA); //We want to write our RH and temp data to the ENV register
  Wire.write(envData[0]);
  Wire.write(envData[1]);
  Wire.write(envData[2]);
  Wire.write(envData[3]);
}

//Reads from a give location from the CSS811
byte CCS811::readRegister(byte addr)
{
  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR); //ITBP add
  Wire.write(addr);
  Wire.endTransmission();

  //Wire.requestFrom(CCS811_ADDR, 1);
  Wire.requestFrom(_I2C_ADDR, 1);  //ITBP add

  return (Wire.read());
}

//Write a value to a spot in the CCS811
void CCS811::writeRegister(byte addr, byte val)
{
  //Wire.beginTransmission(CCS811_ADDR);
  Wire.beginTransmission(_I2C_ADDR); //ITBP add
  Wire.write(addr);
  Wire.write(val);
  Wire.endTransmission();
}

unsigned int CCS811::getCO2()
{
	return (CO2);
}
unsigned int CCS811::gettVOC()
{
	return (tVOC);
}

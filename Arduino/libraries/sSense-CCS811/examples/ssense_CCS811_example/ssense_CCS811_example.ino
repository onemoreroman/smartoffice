/* s-Sense CCS811 I2C sensor breakout example - v1.0/20190524. 
 * 
 * Compatible with:
 *		s-Sense CCS811 I2C sensor breakout [PN: SS-CCS811#I2C, SKU: ITBP-6004], info https://itbrainpower.net/sensors/CCS811-CO2-TVOC-I2C-sensor-breakout 
 *		s-Sense CCS811 + HDC2010 I2C sensor breakout [PN: SS-HDC2010+CCS811#I2C, SKU: ITBP-6006], info https://itbrainpower.net/sensors/CCS811-HDC2010-CO2-TVOC-TEMPERATURE-HUMIDITY-I2C-sensor-breakout
 *
 * Reading CO2 and tVOC values example (pulling at 2sec) - based on test software (Beerware license) written by Nathan Seidle from SparkFun Electronics. 
 * Thank you Nathan! Great job! 
 * 
 * We've ported Nathan's functions into a class, add some variables, functions and fuctionalities.
 * 
 * 
 * Mandatory wiring:
 *    Common for 3.3V and 5V Arduino boards:
 *        sensor I2C SDA  <------> Arduino I2C SDA
 *        sensor I2C SCL  <------> Arduino I2C SCL
 *        sensor GND      <------> Arduino GND
 *        sensor PAD6     <------> Arduino D5 or Arduino GND [connecting to GND the CCS811 sensor will be always ON]
 *    For Arduino 3.3V compatible:
 *        sensor Vin      <------> Arduino 3.3V
 *    For Arduino 5V compatible:
 *        sensor Vin      <------> Arduino 5V
 * 
 * Leave other sensor PADS not connected.
 * 
 * SPECIAL note for some ARDUINO boards:
 *        SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
 *        SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro
 * 
 * WIRING WARNING: wrong wiring may damage your Arduino board MCU or your sensor! Double check what you've done.
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
 * Dragos Iosub, Bucharest 2019.
 * https://itbrainpower.net
 */

#define SERIAL_SPEED  19200

#include <sSense-CCS811.h>

CCS811 ssenseCCS811;


void setup()
{
  DebugPort.begin(SERIAL_SPEED);
  delay(5000);
  DebugPort.println("s-Sense CCS811 I2C sensor.");
  if(!ssenseCCS811.begin(uint8_t(I2C_CCS811_ADDRESS), uint8_t(CCS811_WAKE_PIN), driveMode_1sec))
    DebugPort.println("Initialization failed.");
}

void loop()
{ 
  ssenseCCS811.setEnvironmentalData((float)(21.102), (float)(57.73));  // replace with temperature and humidity values from HDC2010 sensor
  /*if (ssenseCCS811.dataAvailable()){
  {
    ssenseCCS811.readAlgorithmResults(); //Calling this function updates the global tVOC and CO2 variables

    .....DebugPort.print("CO2[");
    DebugPort.print(ssenseCCS811.getCO2());
    DebugPort.print("] tVOC[");
    DebugPort.print(ssenseCCS811.gettVOC());
    DebugPort.print("] millis[");
    DebugPort.print(millis());
    DebugPort.print("]");
    DebugPort.println();
  }
  */
  if (ssenseCCS811.checkDataAndUpdate())
  {
    DebugPort.print("CO2[");
    DebugPort.print(ssenseCCS811.getCO2());
    DebugPort.print("] tVOC[");
    DebugPort.print(ssenseCCS811.gettVOC());
    DebugPort.print("] millis[");
    DebugPort.print(millis());
    DebugPort.print("]");
    DebugPort.println();
  }
  else if (ssenseCCS811.checkForError())
  {
    ssenseCCS811.printError();
  }

  delay(2000);
}

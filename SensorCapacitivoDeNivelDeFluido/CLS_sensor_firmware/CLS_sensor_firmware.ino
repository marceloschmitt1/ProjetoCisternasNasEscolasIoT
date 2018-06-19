// written by Y. de Haas <http://ytec3D.com>
// latest version (V1.00) written: 11-10-2014 (dd-mm-yyyy)(!like it should be everywhere!)

/*
 This code is used for the capacitive level sensor to measure the capacitance of the sensor and tie a value to that capacitance (either as a percentage or a distance)
 This code uses the Wire library and the capacitivesensor library to figure out the out the capacitance and send it over the I2C bus on request. 
 Also some additional features can be configured through the I2C line.
 
 Download the capacitive sensor here: http://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense
 */

#include <Wire.h>
#include <EEPROM.h>
#include <CapacitiveSensor.h>

CapacitiveSensor   csSensor = CapacitiveSensor(7,8);        // high (50M+) resistor between pins 7 and 8. Sensor attached to pin 8                           

//constants
const int readingDelayTime = 2000; //the amount of time between each reading

//variables
unsigned long startTime;
word elapsedTime; 
word elapsedTimeConstrained;
word CLSLevelRaw;
byte CLSLevelPercentage;
word CLSLevelDistancemm;
word CLSLevelDistancewritemm;
word CLSLowerLimit;
word CLSUpperLimit;
word tempval;
byte I2CLastRequest;
byte sendValue[2];


void setup()
{
  Wire.begin(42);                   // join i2c bus with address #42
  Wire.onRequest(requestEvent);     // register event
  Wire.onReceive(receiveEvent);

  csSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate, we don't need it.
  csSensor.set_CS_Timeout_Millis(5000);           //raise timeout to allow for a more accurate reading

  //Serial.begin(9600);

  //fetch high, low and length from EEPROM
  CLSLowerLimit = word(EEPROM.read(40), EEPROM.read(41));
  CLSUpperLimit = word(EEPROM.read(50), EEPROM.read(51));
  CLSLevelDistancemm = word(EEPROM.read(60), EEPROM.read(61));

  //debug default values
  //CLSLowerLimit = 400;
  //CLSUpperLimit = 800;
  //CLSLevelDistancemm = 750;
}

void loop()
{  
  startTime = millis();
  long total1 = csSensor.capacitiveSensor(100); //run 100 measure samples
  elapsedTime = millis() - startTime; //see how much time has passed (the more time has passed, the higher the capacitance was.

  //convert the read value from the array to the byte value, the distance and the capacitance
  //check for values too high or low, make the value upper and lower limits if it exceeds the value
  elapsedTimeConstrained = constrain(elapsedTime, CLSLowerLimit, CLSUpperLimit);
  CLSLevelDistancewritemm = map(elapsedTimeConstrained, CLSLowerLimit, CLSUpperLimit, 0, CLSLevelDistancemm);
  CLSLevelPercentage = map(elapsedTimeConstrained, CLSLowerLimit, CLSUpperLimit, 0 , 255);

  //for debug purposes when sensor is directly connected to serial monitor
  /*Serial.print(elapsedTime);       // print the value to serial port
   Serial.print(" mS;  ");         // print units and carriage return
   Serial.print(CLSLevelDistancewritemm);       // print the value to serial port
   Serial.print(" mm;  ");         // print units and carriage return
   Serial.print(CLSLevelPercentage);       // print the value to serial port
   Serial.println(" ");         // print units and carriage return*/

  //delay at the end of the program to lower measure frequency
  delay(readingDelayTime);
}






// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  switch (I2CLastRequest)
  {
  case 10: //case percentage request, write back the answer as a byte where 0 = 0 and 255 = 100%
    Wire.write(CLSLevelPercentage);
    break;

  case 20: //case distance request, write back 2 bytes containing the distance in mm
    sendValue[0] = highByte(CLSLevelDistancewritemm);//byte(CLSLevelDistancewritemm >> 8);
    sendValue[1] = lowByte(CLSLevelDistancewritemm);//byte(CLSLevelDistancewritemm);
    Wire.write(sendValue,2);
    break;

  case 30: //case raw request, write back 2 bytes where the value is in ms
    sendValue[0] = highByte(elapsedTime);//byte(elapsedTime >> 8);
    sendValue[1] = lowByte(elapsedTime);//byte(elapsedTime);
    Wire.write(sendValue,2);
    break;
  
  case 70:
    sendValue[0] = highByte(CLSLowerLimit);//byte(CLSLevelDistancewritemm >> 8);
    sendValue[1] = lowByte(CLSLowerLimit);//byte(CLSLevelDistancewritemm);
    Wire.write(sendValue,2);
  break;
  
  case 80:
    sendValue[0] = highByte(CLSUpperLimit);//byte(CLSLevelDistancewritemm >> 8);
    sendValue[1] = lowByte(CLSUpperLimit);//byte(CLSLevelDistancewritemm);
    Wire.write(sendValue,2);
  break;
  
  case 90:
    sendValue[0] = highByte(CLSLevelDistancemm);//byte(CLSLevelDistancewritemm >> 8);
    sendValue[1] = lowByte(CLSLevelDistancemm);//byte(CLSLevelDistancewritemm);
    Wire.write(sendValue,2);
  break;

  default:
    Wire.write(0);
  }
}

void receiveEvent(int bytesreceived)
{
  //see if there is the right amount of bytes written
  if (bytesreceived >= 1 & bytesreceived <= 3)
  {
    //read function byte
    byte wirefunction = Wire.read();
    switch (wirefunction)
    {
    case 10: //case percentage request, write back the answer as a byte where 0 = 0 and 255 = 100%
      I2CLastRequest = 10;
      break;

    case 20: //case distance request, write back 2 bytes containing the distance in mm
      I2CLastRequest = 20;
      break;

    case 30: //case raw request, write back 2 bytes where the value is in ms
      I2CLastRequest = 30;
      break;

    case 40: //case note low fluid point, call function setLowpoint()
      setLowPoint();
      break;

    case 50: //case note high fluid point, call function setHighpoint()
      setHighPoint();
      break;

    case 60: //case set sensor length, read value in 2 bytes and call function setSensorLength() in millimeters
      word setSensorLengthmm;
      if (Wire.available() > 1) //if there are 2 bytes in queue)
      {
        setSensorLengthmm = word(Wire.read(),Wire.read()); //receive high byte
        CLSLevelDistancemm = setSensorLengthmm;
        setSensorLength(setSensorLengthmm); //set sensor length
      }
      break;

    case 70: //read sensor low point
      I2CLastRequest = 70;
      break;

    case 80: //read sensor low point
      I2CLastRequest = 80;
      break;

    case 90: //read sensor low point
      I2CLastRequest = 90;
      break;

    }
  }
}



//set low fluid point and write to EEPROM to make the setting non-volatile
void setLowPoint()
{
  //mark the current capacitance as the lowest value
  CLSLowerLimit = elapsedTime;

  //write to EEPROM
  EEPROM.write(40,highByte(CLSLowerLimit));
  EEPROM.write(41,lowByte(CLSLowerLimit));  
}

//set high fluid point and write to EEPROM to make the setting non-volatile
void setHighPoint()
{
  //mark the current capacitance as the lowest value
  CLSUpperLimit = elapsedTime;

  //write to EEPROM
  EEPROM.write(50,highByte(CLSUpperLimit));
  EEPROM.write(51,lowByte(CLSUpperLimit)); 
}

//set the length in mm the sensor functions at and save this in the EEPROM to make this setting non-volatile
void setSensorLength(word setmillimeters)
{
  EEPROM.write(60, highByte(setmillimeters));
  EEPROM.write(61, lowByte(setmillimeters));
}









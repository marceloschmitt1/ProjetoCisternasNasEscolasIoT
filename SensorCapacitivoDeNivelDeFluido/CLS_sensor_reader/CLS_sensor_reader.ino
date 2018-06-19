// written by Y. de Haas <http://ytec3D.com>
// latest version (V1.00) written: 11-10-2014 (dd-mm-yyyy)(!like it should be everywhere!)

/*
 This code is used for the capacitive level sensor master. It is used to measure values and to set sensor values and settings. Through the serial window, 
 several commands can be issued (Case Sensitive);
 *'L': set low value                (the sensor will set the lower limit to the current raw value)
 *'H': Set high value               (the sensor will set the upper limit to the current raw value)
 *'S': Set sensor length            (the sensor will set the sensor length to the value specified in this programm: setValueMM)
 *'l': Read low value               (the sensor will respond with the current lower limit)
 *'h': Read high value              (the sensor will respond with the current upper limit)
 *'m': Read sensor length           (the sensor will respond with the current sensor length in mm)
 *'R': Read raw sensor value        (the sensor will respond with the recently measured raw value)
 *'P': Read sensor 0-255 value      (the sensor will respond with the recently measured 0-255 value)
 *'M': Read sensor distance value   (the sensor will respond with the recently measured fluid level in mm)
*/

#include <Wire.h>
byte readValue[2];

word setValueMM = 500;//750; //the value in mm of the length of the sensor

void setup()
{
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
}

word reading = 0;

void loop()
{
  while(Serial.available())
  {
    char c = Serial.read();
    if (c == 'L') //set low sensor value
    {
      Wire.beginTransmission(42);
      Wire.write(40);
      Wire.endTransmission();
      Serial.println("Set low point requested");
    }
    else if (c == 'H') //set high sensor value
    {
      Wire.beginTransmission(42);
      Wire.write(50);
      Wire.endTransmission();
      Serial.println("Set high point requested");
    }
    else if (c == 'R') //read raw value
    {
      Wire.beginTransmission(42);
      Wire.write(30);
      Wire.endTransmission();
      Wire.requestFrom(42, 2);
      if (Wire.available() > 1)
      {
        readValue[0] = Wire.read();
        readValue[1] = Wire.read();
      }
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp = readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read raw BR1: ");
      Serial.print(readValue[0]);
      Serial.print(" BR2: ");
      Serial.print(readValue[1]);
      Serial.print(" t: ");
      Serial.println(readtemp);
    }
    else if (c == 'M') //read distance in mm
    {
      Wire.beginTransmission(42);
      Wire.write(20);
      Wire.endTransmission();
      Wire.requestFrom(42, 2);
      readValue[0] = Wire.read();
      readValue[1] = Wire.read();
      delay(100);
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp =  readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read mm B1: ");
      Serial.print(readValue[0]);
      Serial.print(" B2: ");
      Serial.print(readValue[1]);
      Serial.print(" t: ");
      Serial.println(readtemp);
    }
    else if (c == 'P') //read percentage byte
    {
      Wire.beginTransmission(42);
      Wire.write(10);
      Wire.endTransmission();
      Wire.requestFrom(42, 1);
      readValue[0] = Wire.read();
      delay(100);
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp =  readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read % t: ");
      Serial.println(readValue[0]);
    }
    else if (c == 'S') //set sensor length to distance
    {
      byte tempSendValue[3] = {60, highByte(setValueMM), lowByte(setValueMM)};
      Wire.beginTransmission(42);
      Wire.write(tempSendValue, 3);
      Wire.endTransmission();
      //decode incoming transmission into 1 word and print all values over serial
      Serial.print("length set to ");
      Serial.println(setValueMM);
    }
    else if (c == 'l') //read lower limmit raw value
    {
      Wire.beginTransmission(42);
      Wire.write(70);
      Wire.endTransmission();
      Wire.requestFrom(42, 2);
      readValue[0] = Wire.read();
      readValue[1] = Wire.read();
      delay(100);
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp =  readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read low limit B1: ");
      Serial.print(readValue[0]);
      Serial.print(" B2: ");
      Serial.print(readValue[1]);
      Serial.print(" r: ");
      Serial.println(readtemp);
    }
    else if (c == 'h') //read upper limmit raw value
    {
      Wire.beginTransmission(42);
      Wire.write(80);
      Wire.endTransmission();
      Wire.requestFrom(42, 2);
      readValue[0] = Wire.read();
      readValue[1] = Wire.read();
      delay(100);
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp =  readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read high limit B1: ");
      Serial.print(readValue[0]);
      Serial.print(" B2: ");
      Serial.print(readValue[1]);
      Serial.print(" r: ");
      Serial.println(readtemp);
    }
    else if (c == 'm') //read tube length mm
    {
      Wire.beginTransmission(42);
      Wire.write(90);
      Wire.endTransmission();
      Wire.requestFrom(42, 2);
      readValue[0] = Wire.read();
      readValue[1] = Wire.read();
      delay(100);
      //decode incoming transmission into 1 word and print all values over serial
      word readtemp = readValue[0];
      readtemp =  readtemp << 8;
      readtemp |= readValue[1];
      Serial.print("Read tube length B1: ");
      Serial.print(readValue[0]);
      Serial.print(" B2: ");
      Serial.print(readValue[1]);
      Serial.print(" r: ");
      Serial.println(readtemp);
    }
  }
}



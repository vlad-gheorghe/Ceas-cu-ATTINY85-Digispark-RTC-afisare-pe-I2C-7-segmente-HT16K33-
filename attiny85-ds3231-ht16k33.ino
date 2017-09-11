#include <TinyWireM.h> //https://github.com/adafruit/TinyWireM
#include <Tiny_LEDBackpack.h> //https://github.com/millerlp/Tiny_LEDBackpack

Tiny_7segment sevenseg = Tiny_7segment();
//int led = 10; // indicator LED for ATtiny84. Connect with 100-470ohm resistor

#define i2c_addr 0x70 // stock address for Adafruit 7-segment LED backpack
#define keyHor PB1
#define keyMin PB3 

#define DS3231_I2C_ADDRESS 0x68

byte decToBcd(byte val){
  return ( (val/10*16) + (val%10) );
}
 
byte bcdToDec(byte val){
  return ( (val/16*10) + (val%16) );
}

void setDateDs3231(byte second,  
                   byte minute,        // 0-59
                   byte hour,          // 1-23
                   byte dayOfWeek,     // 1-7
                   byte dayOfMonth,    // 1-28/29/30/31
                   byte month,         // 1-12
                   byte year)          // 0-99
{
   TinyWireM.beginTransmission(DS3231_I2C_ADDRESS);
   TinyWireM.write(0);
   TinyWireM.write(decToBcd(second));    
   TinyWireM.write(decToBcd(minute));
   TinyWireM.write(decToBcd(hour));     
   TinyWireM.write(decToBcd(dayOfWeek));
   TinyWireM.write(decToBcd(dayOfMonth));
   TinyWireM.write(decToBcd(month));
   TinyWireM.write(decToBcd(year));
   TinyWireM.endTransmission();
}
 
void getDateDs3231(byte *second,
          byte *minute,
          byte *hour,
          byte *dayOfWeek,
          byte *dayOfMonth,
          byte *month,
          byte *year)
{
 
  TinyWireM.beginTransmission(DS3231_I2C_ADDRESS);
  TinyWireM.write(0);
  TinyWireM.endTransmission();
 
  TinyWireM.requestFrom(DS3231_I2C_ADDRESS, 7);
 
  *second     = bcdToDec(TinyWireM.read() & 0x7f);
  *minute     = bcdToDec(TinyWireM.read());
  *hour       = bcdToDec(TinyWireM.read() & 0x3f); 
  *dayOfWeek  = bcdToDec(TinyWireM.read());
  *dayOfMonth = bcdToDec(TinyWireM.read());
  *month      = bcdToDec(TinyWireM.read());
  *year       = bcdToDec(TinyWireM.read());
}
 
void setINT(){  
  TinyWireM.beginTransmission(DS3231_I2C_ADDRESS);
  TinyWireM.write(0x0E);
  TinyWireM.write(0x0);
  TinyWireM.endTransmission();
}


void setup() {
   pinMode(keyHor, INPUT_PULLUP);
  pinMode(keyMin, INPUT_PULLUP);
 
 // pinMode(led, OUTPUT);
 
  sevenseg.begin(i2c_addr); // initialize HT16K33 controller

  sevenseg.clear(); // clear all digits on display
  sevenseg.setBrightness(2);
  sevenseg.writeDisplay(); // push data to display
  delay(1000);
}

void loop(){
 byte second, minute, hour, dayOfWeek, dayOfMonth, month, year; 
  getDateDs3231(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); 


 sevenseg.writeDigitRaw(2,2);
 sevenseg.writeDisplay();
 delay(500);
 sevenseg.writeDigitRaw(2,0);
 sevenseg.writeDisplay();
 delay(500);
 
 if (!digitalRead(keyHor) ){   
      second = 0;     
      hour++;  
      if (hour > 23) hour = 0;  
      setDateDs3231(second, minute, hour, dayOfWeek, dayOfMonth, month, year); 
      delay(200);
  }
  if (!digitalRead(keyMin)){  
      second = 0;
      minute++;
      if (minute > 59) minute = 0;
      setDateDs3231(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
      delay(200);
  }
  
  sevenseg.writeDigitNum(0, (hour/10));
 sevenseg.writeDigitNum(1, hour % 10);
  sevenseg.writeDigitNum(3, minute /10);
  sevenseg.writeDigitNum(4, minute % 10);

  sevenseg.writeDisplay();
}



/*
Arduino waiting for code from PC (channel selection) reads values from a Microchip MCP-3424 18 bit I2C ADC.
Transmits those values over serial connection.

*/

#include <Wire.h>
#define MCP3424_adres 0X68
#define MCP3424_wzmocnienie 0X03
#define MCP3424_18bit 0x03

byte topbyte;
byte midbyte;
byte lowbyte;
byte configbyte;
long top;
long mid;
long low;
int32_t wynik;
long wynik_konc;


int led2 = 10;
int led3 = 12;

uint8_t ADCconfig;

int8_t read() {


  configbyte = 0x00;        //clear config byte
  configbyte |= 1 << 7;

   do { // wait for update last conversion
    
    
    Wire.requestFrom(MCP3424_adres, 4);

    if (Wire.available() == 4) {
      
    while (Wire.available()) {

      topbyte = Wire.read();
      topbyte &= 0x03; // clear last 6 bits
      midbyte = Wire.read();
      lowbyte = Wire.read();
      configbyte = Wire.read();

    }

    top = topbyte;
    mid = midbyte;
    low = lowbyte;

    wynik = (top << 16) + (mid << 8) + low; // connect 3 bytes
    
    }
    else
    {
     return false;
     Serial.println("ERROR"); 
      
    }
   }
   while ((configbyte & (1 << 7)));
  




}

void write(uint8_t config) {

  Wire.beginTransmission(MCP3424_adres);
  Wire.write(config);
  Wire.endTransmission();

}

void setup() {

  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  Serial.begin(9600);

  Wire.begin();



  digitalWrite(led2, HIGH);  // power is on


}

void loop() {



  if (Serial.available()) {

    char ch = Serial.read();

    ADCconfig  &= 0; //clear ADCconfig byte
    ADCconfig  |= 0X80 | 0X00 << 5 | 0x00 << 4 | MCP3424_18bit << 2 | MCP3424_wzmocnienie; //initiate new conversion, select channel 1, one shot conversion mode, set resolution to 18bit, set gain to 8

    if (ch == '1') {


      digitalWrite(led3, HIGH);

      ADCconfig |=  0x00 << 5; //select channel 1

      write(ADCconfig);



      read();

      long test = pow(2, 17);
      long set = pow(2, 18);
      if (wynik >= test) {

        wynik_konc = wynik - set;

      }
      else {


        wynik_konc = wynik;

      }

      Serial.flush();
      Serial.print("CH1 ");
      Serial.print(wynik_konc);
      Serial.println(" ");
      digitalWrite(led3, LOW);

    }

    if (ch == '2') {


      digitalWrite(led3, HIGH);

      ADCconfig |= 0x01 << 5; //select channel 2

      write(ADCconfig);

      read();

      long test = pow(2, 17);
      long set = pow(2, 18);
      if (wynik >= test) {

        wynik_konc = wynik - set;

      }
      else {


        wynik_konc = wynik;

      }
      Serial.flush();
      Serial.print("CH2 ");
      Serial.print(wynik_konc);
      Serial.println(" ");


      digitalWrite(led3, LOW);
    }


  }


}

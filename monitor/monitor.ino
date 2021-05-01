//www.elegoo.com
//2016.12.9

/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include <Wire.h>

// Define Pins
#define BLUE 3
#define GREEN 5
#define RED 6

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

String inData;
// define variables
int redValue;
int greenValue;
int blueValue;

int recentCPULoad = 0;
int recentCPUTemp = 0;
int recentRAMLoad = 0;
int recentRAMUsed = 0;

void setLight() {

  if (recentCPULoad > 80 || recentCPUTemp > 100 || recentRAMLoad > 80) {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
  }
  else if (recentCPULoad > 60 || recentCPUTemp > 80 || recentRAMLoad > 60) {
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }
  else {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, LOW);
  }
  
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  Serial.begin(9600);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, LOW);
}

void loop() {
    //lcd.clear();
  while (Serial.available() > 0)
    {
      
      char received = Serial.read();
      
      lcd.setCursor(0,0);

      inData += received; 
      
      if (received == 42) // *
      {
        inData.remove(inData.length() - 1, 1);
        lcd.setCursor(0,0);
        lcd.print("CPU: " + inData + "% ");

        recentCPULoad = inData.toInt();

        inData = ""; 
      }
      if (received == 35) // #
      {
        inData.remove(inData.length() - 1, 1);
        lcd.setCursor(10,0);
        lcd.print(inData + char(223) + "C");
        recentCPUTemp = inData.toInt();
        inData = ""; 
      }
      
      if (received == 36) // $
      {
        inData.remove(inData.length() - 1, 1);
        lcd.setCursor(0,1);
        lcd.print("RAM: " + inData + "% ");
        recentRAMLoad = inData.toInt();
        inData = ""; 
      }
      if (received == 38) // &
      {
        inData.remove(inData.length() - 1, 1);
        lcd.setCursor(11,1);
        lcd.print(inData + "GB");
        inData = ""; 
      }
    }
    setLight();
}

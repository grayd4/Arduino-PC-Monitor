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

 potential rgb source: https://forum.arduino.cc/t/color-changing-rgb-led-rainbow-spectrum/8561
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

// Number of colors used for animating, higher = smoother and slower animation)
int numColors = 511;
int counter = 0;

// The combination of numColors and animationDelay determines the
// animation speed, I recommend a higher number of colors if you want
// to slow down the animation. Higher number of colors = smoother color changing.
int animationDelay = 10; // number milliseconds before RGB LED changes to next color


// PC vars
int recentCPULoad = 0;
int recentCPUTemp = 0;
int recentRAMLoad = 0;
int recentRAMUsed = 0;

void setLight(unsigned char red, unsigned char green, unsigned char blue) {

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
    //digitalWrite(RED, LOW);
    //digitalWrite(GREEN, HIGH);
    //digitalWrite(BLUE, LOW);
    // If we aren't d isplaying a warning light, just continue to cycle RGB
    setColor(red, green, blue);
  }
  
}

void setColor(unsigned char red, unsigned char green, unsigned char blue)
{
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}

// Translate all our params to a hard rgb value
long HSBtoRGB(float _hue, float _sat, float _brightness) {
  float red = 0.0;
  float green = 0.0;
  float blue = 0.0;
  
  if (_sat == 0.0) {
    red = _brightness;
    green = _brightness;
    blue = _brightness;
  } else {
    if (_hue == 360.0) {
      _hue = 0;
    }

    int slice = _hue / 60.0;
    float hue_frac = (_hue / 60.0) - slice;

    float aa = _brightness * (1.0 - _sat);
    float bb = _brightness * (1.0 - _sat * hue_frac);
    float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
    
    switch(slice) {
      case 0:
        red = _brightness;
        green = cc;
        blue = aa;
        break;
      case 1:
        red = bb;
        green = _brightness;
        blue = aa;
        break;
      case 2:
        red = aa;
        green = _brightness;
        blue = cc;
        break;
      case 3:
        red = aa;
        green = bb;
        blue = _brightness;
        break;
      case 4:
        red = cc;
        green = aa;
        blue = _brightness;
        break;
      case 5:
        red = _brightness;
        green = aa;
        blue = bb;
        break;
      default:
        red = 0.0;
        green = 0.0;
        blue = 0.0;
        break;
    }
  }
  long ired = red * 255.0;
  long igreen = green * 255.0;
  long iblue = blue * 255.0;
  
  return long((ired << 16) | (igreen << 8) | (iblue));
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
  // This part takes care of displaying the
  // color changing in reverse by counting backwards if counter
  // is above the number of available colors  
  float colorNumber = counter > numColors ? counter - numColors: counter;
  
  // Play with the saturation and brightness values
  // to see what they do
  float saturation = 1; // Between 0 and 1 (0 = gray, 1 = full color)
  float brightness = .05; // Between 0 and 1 (0 = dark, 1 is full brightness)
  float hue = (colorNumber / float(numColors)) * 360; // Number between 0 and 360
  long color = HSBtoRGB(hue, saturation, brightness); 

  // Get the red, blue and green parts from generated color
  int red = color >> 16 & 255;
  int green = color >> 8 & 255;
  int blue = color & 255;
  
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
  setLight(red, green, blue);

  // Counter can never be greater then 2 times the number of available colors
  // the colorNumber = line above takes care of counting backwards (nicely looping animation)
  // when counter is larger then the number of available colors
  counter = (counter + 1) % (numColors * 2);

  // If you uncomment this line the color changing starts from the
  // beginning when it reaches the end (animation only plays forward)
  // counter = (counter + 1) % (numColors);

  delay(animationDelay);
}

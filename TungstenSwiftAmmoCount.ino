/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the Bounce2 library found here :
// https://github.com/thomasfredericks/Bounce-Arduino-Wiring
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Bounce2.h>
#include <SPI.h>
#include <Wire.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the Adafruit-GFX library found here :
// https://github.com/adafruit/Adafruit-GFX-Library
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Adafruit_GFX.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the Adafruit_SSD1306 library found here :
// https://github.com/adafruit/Adafruit_SSD1306
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH         128 // OLED display width, in pixels
#define SCREEN_HEIGHT        64 // OLED display height, in pixels

#define OLED_RESET            4
#define PIN_DARTCOUNT        A2   // (Green)  PIN listening to dart counter event 

#define PIN_DARTCOUNT_UP     A0   // (Orange) PIN Listening to Dart Count Up
#define PIN_DARTCOUNT_DOWN   A1   // (Grey)   PIN Listening to Dart Count Up
// Note                      A4      (Blue)   are used by the OLED SDA (Blue)
// Note                      A5      (Yellow) are used by the OLED SCL (Yellow)

int     ammoLimit          = 15;           // default set as 15 dart mag
int     dartLeft           = ammoLimit;    // track amount of dart in mag, same default value as of ammoLimit
boolean isStart            = true;

//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Bounce btnDartReset      = Bounce();
Bounce dartCountSwitch   = Bounce();
Bounce btnDartCountUp    = Bounce();
Bounce btnDartCountDown  = Bounce();

void updateDisplay(int ammo) {

  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);

  if (isStart) {
    display.setCursor(4,0);
    display.print("SET M");
  } else {
    display.setCursor(11,0);
    display.print("SHOT");
  }

  display.drawLine(0 , 17, 64, 17, WHITE);
  display.drawLine(5 , 19, 59, 19, WHITE);
  display.drawLine(10, 21, 54, 21, WHITE);
  display.drawLine(15, 23, 49, 23, WHITE);
  display.drawLine(20, 25, 44, 25, WHITE);
  display.drawLine(25, 27, 39, 27, WHITE);
  
  display.drawRect(0, 96, 64, 32, WHITE);
  
  display.setTextSize(1);
  display.setCursor(14,102);
  display.println("MODDED"); 
  display.setCursor(8,114);
  display.println("TUNGSTEN");

  display.fillRect(0, 32, 64, 60, WHITE);

  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(2,80);
  display.println("Ammo Count");  
  
  if (ammo == 1) {
    display.setCursor(4,40);
    display.setTextSize(5);
    display.println("*L");
  } else {
    display.setCursor(4,40);
    display.setTextSize(5);
    if (ammo < 10)
      display.print(0);  
    display.println(ammo);
  }

  display.display();
}

void reLoadDisplay() {

  display.clearDisplay();  
  display.setTextColor(WHITE);
  display.drawRect(0, 0, 64, 128, WHITE);

  display.setTextSize(6);
  display.setCursor(2,3);
  display.println("R");
  display.setCursor(2,53);
  display.println("E");

  display.setTextSize(4);
  display.setCursor(8,99);
  display.println("*");
  display.setCursor(40,4);
  display.println("L");
  display.setCursor(40,34);
  display.println("O");
  display.setCursor(40,64);
  display.println("A");
  display.setCursor(40,94);
  display.println("D");

  display.display();
}

void setup() {
  
  pinMode(PIN_DARTCOUNT, INPUT_PULLUP);       // PULLUP
  dartCountSwitch.attach(PIN_DARTCOUNT);
  dartCountSwitch.interval(5);

  pinMode(PIN_DARTCOUNT_UP, INPUT_PULLUP);    // PULLUP
  btnDartCountUp.attach(PIN_DARTCOUNT_UP);
  btnDartCountUp.interval(5);

  pinMode(PIN_DARTCOUNT_DOWN, INPUT_PULLUP);  // PULLUP
  btnDartCountDown.attach(PIN_DARTCOUNT_DOWN);
  btnDartCountDown.interval(5);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)  
  display.setRotation(1); 
  display.clearDisplay();

  updateDisplay(dartLeft);
}

void loop() {

  dartCountSwitch.update();
  btnDartCountUp.update();
  btnDartCountDown.update();
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Listen to Dart Count Up
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  if (btnDartCountUp.fell()) { // pressed, dart count up
    if (isStart) {
      ammoLimit++;
    }

    if (digitalRead(PIN_DARTCOUNT_DOWN) == LOW) {
      if (dartLeft < ammoLimit) {
        dartLeft++;
      }
    } else {
      dartLeft = ammoLimit;
    }    
    updateDisplay(dartLeft);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Listen to Dart Count Down
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  if (btnDartCountDown.fell()) { // pressed, dart count down
    if (isStart) {
      if (ammoLimit > 1) {
        dartLeft = --ammoLimit;
      }
    } else {
      if (dartLeft > 1) {
        --dartLeft;
      }
    }
    updateDisplay(dartLeft);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Listen to Dart Count
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  if (dartCountSwitch.fell()) { // pressed
      if (isStart) {
        isStart= false;    
      } else {
        if (dartLeft > 1) {
          dartLeft--;
        } else {
          dartLeft = ammoLimit;
          reLoadDisplay();
        }
      }
  } else if (dartCountSwitch.rose()) { // released
    updateDisplay(dartLeft);
  }
}

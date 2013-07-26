/* Prototype on Arduino UNO or compatible
 * SETUP:
 *
 *    Arduino RX from GRBL/XStepper Board TX with 220ohm Resistor
 *    Arduino GND from GRBL/XStepper Board 
 *    Arduino A4 SDA  to I2C LCD Board 
 *    Arduino A5 SCL  to I2C LCD Board 
 *    Arduino VCC to  XLCD Board 
 *    Arduino GND to  XLCD Board 
 *
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 */

/* ATtiny85 as an Serial Monitor to display information on LCD Display
 * SETUP:
 *
 *    (1) ATtiny D5 = RESET                            (8) ATtiny Pin -- = VCC (2.7-5.5V)
 *    (2) ATtiny D3 = TX (serial not use!)             (7) ATtiny D2 = SCK (I2C LCD)
 *    (3) ATtiny D4 = RX (serial monitor)              (6) ATtiny D1 = switch to control
 *    (4) ATtiny -- = GND                              (5) ATtiny D0 = SDA (I2C LCD) 
 *
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * LiquidCrystal_I2C lib was modified for ATtiny - on Playground with TinyWireM lib.
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 */

//#define DEBUG
//#include <MemoryFree.h>
#include <Wire.h>                // I2C Communication (Attiny must use TinyWireM
#include <LCD.h>
#include <LiquidCrystal_I2C.h>   // LCD over I2C
#include <SoftwareSerial.h>      // listen on TX Line from GRBL to PC

SoftwareSerial mySerial(10, -1); // RX, TX

#define BUTTON          1     // Button to control

//LiquidCrystal_I2C lcd(LCD_ADDR); // 16 x 2 Display

#define LCD_ADDR        0x27  // I2C LCD Address
#define LCD_LETTERS  20
#define LCD_ROWS      4
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

//Button to change menu .. not realized yet
//menu: 
// m = display machine position
// w = display work position
// d = display debug display
char menus[4] = "mwd";
int mchange = 0;
char option = menus[0]; // has default option
int  menusize = (sizeof(menus)/sizeof(char *)); //array size

void setup() 
{ 

  lcd.begin(LCD_LETTERS,LCD_ROWS); 
  
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("XstepperLCD 0.1");
  
  Serial.begin(9600);
  Serial.println("XStepperLCD 0.1");
  
  delay(1000);
  
  Serial.print("Option: ");Serial.println(option);
  lcd.clear();
  mySerial.begin(9600); // open serial spy line
} 



void loop() 
{ 
  String buffer = "";
  char character;

  // Try to read from serial spy line and send
  // this to normal Serial fro debug
  while(mySerial.available() > 0) {
      character = mySerial.read();
      if(character == '\n'){
        Serial.println("Read: " + buffer);
        // try to parse this line
        parse_line(buffer);
        buffer = "";
      } else {
        buffer.concat(character);
      }
  }
  delay(100);
}//LOOP

// String subtext = getValue(line, ',', 2);
// Split for strings, i.e. split at command an get the third value
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void parse_line( String line )
{
  // <Idle,MPos:5.529,0.560,7.000,WPos:1.529,-5.440,-0.000>

  // State ..
  String state = getValue(getValue(line, ',', 0), '<', 1);  

  // Machine position ...
  String machinepos_x = getValue(getValue(line, ',', 1), ':', 1);
  String machinepos_y = getValue(line, ',', 2);
  String machinepos_z = getValue(line, ',', 3);

  // Working position ...
  String workingpos_x = getValue(getValue(line, ',', 4), ':', 1);
  String workingpos_y = getValue(line, ',', 5);
  String workingpos_z = getValue(getValue(line, ',', 6), '>', 0);

  // return if not state
  if(workingpos_z.length() == 0){
    return;
  } 

  // Display on LCD ... 
  // lcd screen
  // |--------------|
  // State  Z:555.529
  // 000.000  000.000

  // XXX: to use a switch to display workpos or other things :)
  // lcd.clear();
   if(option == 'm'){
     lcd.setCursor(0,0); // letter, row
     lcd.print(state + "            ");
     lcd.setCursor((LCD_LETTERS-machinepos_z.length()),0);
     lcd.print(machinepos_z);

     lcd.setCursor(0,1);
     lcd.print(machinepos_x);
     lcd.setCursor((LCD_LETTERS-machinepos_y.length()),1);
     lcd.print(machinepos_y);
   }
   if(option == 'w' || LCD_ROWS > 2){
     lcd.setCursor(0,2); // letter, row
     lcd.print("WorkPos:            ");
     lcd.setCursor((LCD_LETTERS-workingpos_z.length()),2);
     lcd.print(workingpos_z);

     lcd.setCursor(0,3);
     lcd.print(workingpos_x);
     lcd.setCursor((LCD_LETTERS-workingpos_y.length()),3);
     lcd.print(workingpos_y);
   }
   if(option == 'd'){
      // Debug Infos
      lcd.print("Debugscreen");
   }
}

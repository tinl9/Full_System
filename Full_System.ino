/* Setup HC12
 * While pin headers are closest to you
 * From HC12 to Seeduino
 * Pin 1 to Power
 * Pin 2 to GND
 * Pin 3 to pin 6
 * pin 4 to pin 7
 * Pin 5 open or Power if transmit receive (GND if configuring)
 * 
 * Setup LCD
 * VSS to Power (can be 3.3v)
 * GND to GND
 * V0 to potentiometer (or gnd)
 * RS to pin 9
 * RW to GND
 * EN to pin 8
 * D0
 * D1
 * D2
 * D3
 * D4 to pin 0
 * D5 to pin 1
 * D6 to pin 2
 * D7 to pin 3
 * A to Power
 * K to GND
 */

#include <LiquidCrystal.h>// include the library code
#include <SoftwareSerial.h>

#define TEMP_HAZARD 36
#define HUMIDITY_HAZARD 90

LiquidCrystal lcd(9, 8, 0, 1, 2, 3); // initialize the library with the numbers of the interface pins (Rs, E, DB4, DB5, DB6, DB7)
SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin

void setup() {
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  Serial.begin(9600);
  HC12.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int j;
  for(int i = 10; i <=100; i+=10){
    j = 100 - i;
    displayData(j, i);
    delay(1000);
  }
  if (isHazardous) {
    sendSignal();
  }
}

void getSensorData(int &humidity, int &temperature) {
  //TODO get humitidy
  //TODO get temperature
}

bool isHazardous(int h, int t){
  if (t < TEMP_HAZARD && h > HUMIDITY_HAZARD) {
    //TODO wake-up microcontroller?
    return true;
  }
  else {
    return false;
  }
}

void displayData(int hum, int temp){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("H: %d   T: %d F",hum, temp);
}

void sendSignal() {
  HC12.write("Something");
}

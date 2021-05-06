/*
 * Transmitter
 * Sends signal based on conditions received from sensor
 * Displays relevant information such as temperature, humidity on LCD screen
 * Performs checks on sensor and transceiver and displays errors, if any
 */
 
/* Setup HC12 (transceiver)
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
 * 
 * AM2320 (temperature/humidity sensor)
 * Pin 1 to VCC
 * Pin 2 to pin 4 (with pullup) SDA
 * Pin 3 to GND
 * Pin 4 to pin 5 (with pullup) SCL
 */

#include <LiquidCrystal.h>// include the library code
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>

#define SET_PIN 10

#define TEMP_HAZARD 36
#define HUMIDITY_HAZARD 95


LiquidCrystal lcd(9, 8, 0, 1, 2, 3); // initialize the library with the numbers of the interface pins (Rs, E, DB4, DB5, DB6, DB7)
SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin
Adafruit_AM2320 AM2320 = Adafruit_AM2320();

void setup() {
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  Serial.begin(9600);
  HC12.begin(9600);
  AM2320.begin();
  pinMode(SET_PIN, OUTPUT);
  checkHC12();
}

void loop() {
  float temperatureC;
  float temperatureF;
  float humidity;
  lcd.clear();
  getSensorData(temperatureC, temperatureF, humidity);
  
//  temperatureF = 30;
//  humidity = 96;

  displaySensorData(temperatureC, temperatureF, humidity);

  if(isHazardous(temperatureF, humidity))
  {
    sendHazardSignal();
  }
  else 
  {
    sendClearSignal();
  }  
  while (HC12.available())          // If HC-12 has data   
  {        
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available())        // If Serial monitor has data
  {      
    HC12.write(Serial.read());      // Send that data to HC-12
  }
  
  checkHC12();

  // Wait 2 seconds between readings:
  delay(2000);

}

void checkHC12(void)
{
// using these lines of code will prevent the program from running unless you open the serial monitor
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }
//  Serial.println("Serial monitor available... OK");

  Serial.print("HC12 available... ");  
//  if (HC12.isListening()) {
//    Serial.println("OK");
//  } else {
//    Serial.println("NOK");  
//  }  
  byte incomingByte;
  String HC12Status = "";
  
  digitalWrite(SET_PIN, LOW); //command mode
  delay(100);
  HC12.print("AT");           //check transmission
  delay(100);
  
  while(HC12.available())     //save transmission status
  {
    incomingByte = HC12.read();
    
    if((char(incomingByte) != '\n') && (char(incomingByte) != '\r'))  //remove new line and carriage return characters
    {
//      Serial.println(incomingByte);      
      HC12Status += char(incomingByte);  //concatenate status
    }       
  }
  //check status and print to serial/lcd
  if(HC12Status == "OK")
  {
    Serial.println("OK");
    lcd.setCursor(8, 1);
    lcd.print("HC12:OK");
  }
  else
  {
    Serial.println("NOK");
    lcd.setCursor(8,1);
    lcd.print("HC12:NOK");
  }
}

void getSensorData(float &temperatureC, float &temperatureF, float &humidity) 
{
  temperatureC = AM2320.readTemperature();
  temperatureF = temperatureC * 1.8 + 32; // converts Celsius to Fahrenheit
  humidity = AM2320.readHumidity();
}

//determine hazardous conditions as set by variables TEMP_HAZARD and HUMIDITY_HAZARD
bool isHazardous(int t, int h)
{
  if (t <= TEMP_HAZARD && h >= HUMIDITY_HAZARD) 
  {
    //TODO wake-up microcontroller?
    return true;
  }
  else
  {
    return false;
  }
}

void sendHazardSignal() 
{
  lcd.setCursor(0,1);
  lcd.print("Hazard");
  Serial.print("Hazardous\n");  
  digitalWrite(SET_PIN, HIGH);  //transparent mode
  delay(100);
  HC12.print("Hazard");
  delay(100);
}

void sendClearSignal()
{
  lcd.setCursor(0,1);
  lcd.print("Clear");
  Serial.print("Clear\n");   
  digitalWrite(SET_PIN,HIGH);
  delay(100);
  HC12.print("Clear");
  delay(100);
}

// Print the temperature and humidity in the Serial Monitor:
void displaySensorData(float tempC, float tempF, float hum)
{
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.print("C  |  ");
  Serial.print(tempF);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.println("F");
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %RH");

  lcd.setCursor(0,0);
  lcd.printf("T: %.0f F", tempF);
  lcd.setCursor(8,0);
  lcd.printf("H: %.0f", hum);
 
}

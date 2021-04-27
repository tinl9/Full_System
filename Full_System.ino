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
 * 
 * AM2320
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
#define HUMIDITY_HAZARD 90


LiquidCrystal lcd(9, 8, 0, 1, 2, 3); // initialize the library with the numbers of the interface pins (Rs, E, DB4, DB5, DB6, DB7)
SoftwareSerial HC12(7,6); //HC12 TX Pin, HC12 RX pin
Adafruit_AM2320 AM2320 = Adafruit_AM2320();

void setup() {
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  Serial.begin(9600);
  HC12.begin(9600);
  AM2320.begin();
  checkHC12();
}

void loop() {
  float temperatureC;
  float temperatureF;
  float humidity;
  getSensorData(temperatureC, temperatureF, humidity);
  
//  temperatureF = 30;
//  humidity = 100;
  displaySensorData(temperatureC, temperatureF, humidity);

  if(isHazardous(temperatureF, humidity))
  {
    sendSignal();
  }
  while (HC12.available())          // If HC-12 has data   
  {        
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while (Serial.available())        // If Serial monitor has data
  {      
    HC12.write(Serial.read());      // Send that data to HC-12
  }
  


  // Wait 2 seconds between readings:
  delay(2000);

}

void checkHC12(void)
{
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Serial monitor available... OK");

  Serial.print("HC12 available... ");  
  if (HC12.isListening()) {
    Serial.println("OK");
  } else {
    Serial.println("NOK");
  }  
  digitalWrite(SET_PIN, LOW);
  HC12.write("AT");
  delay(1000);
  while(HC12.available())
  {
    Serial.write(HC12.read());    
  }
}

void getSensorData(float &temperatureC, float &temperatureF, float &humidity) 
{
  temperatureC = AM2320.readTemperature();
  temperatureF = temperatureC * 1.8 + 32; // converts Celsius to Fahrenheit
  humidity = AM2320.readHumidity();
}

bool isHazardous(int t, int h)
{
  if (t <= TEMP_HAZARD && h >= HUMIDITY_HAZARD) 
  {
    //TODO wake-up microcontroller?
    lcd.setCursor(0,1);
    lcd.print("Hazardous");
    Serial.print("Hazardous\n");
    return true;
  }
  else 
  {
    lcd.setCursor(0,1);
    lcd.print("Not Hazardous");
    return false;
  }
}

void sendSignal() 
{
  digitalWrite(SET_PIN, HIGH);
  HC12.write("Something");
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

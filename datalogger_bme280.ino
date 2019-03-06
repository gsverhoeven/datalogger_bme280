/*
datalogger_bme280.ino

This code shows how to record data from the BME280 environmental sensor
using I2C interface, and write to a microSD card using SPI interface. 

This file is based on an example file, part of the Arduino
BME280 library.

GNU General Public License

Written: March 2019.

Connecting the BME280 Sensor:
Sensor              ->  Board
-----------------------------
Vin (Voltage In)    ->  3.3V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  A4 on Nano
SCK (Serial Clock)  ->  A5 on Nano

 */
#include <SPI.h>
#include <SD.h>

File myFile;

#include <BME280I2C.h>
#include <Wire.h>

#define SERIAL_BAUD 9600

BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

//////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(SERIAL_BAUD);

  while(!Serial) {} // Wait

 Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  Wire.begin();

  while(!bme.begin())
  {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }

  // bme.chipID(); // Deprecated. See chipModel().
  switch(bme.chipModel())
  {
     case BME280::ChipModel_BME280:
       Serial.println("Found BME280 sensor! Success.");
       break;
     case BME280::ChipModel_BMP280:
       Serial.println("Found BMP280 sensor! No Humidity available.");
       break;
     default:
       Serial.println("Found UNKNOWN sensor! Error!");
  }
  pinMode(LED_BUILTIN, OUTPUT);
}

//////////////////////////////////////////////////////////////////
void loop()
{
   printBME280Data(&Serial); // this also saves to microSD
   digitalWrite(LED_BUILTIN, HIGH); // blink led to indicate measurement has been made
   delay(250);
   digitalWrite(LED_BUILTIN, LOW);
   delay(250);
}

//////////////////////////////////////////////////////////////////
void printBME280Data
(
   Stream* client
)
{
   float temp(NAN), hum(NAN), pres(NAN);
   unsigned long my_time;
   
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   my_time = millis();
   bme.read(pres, temp, hum, tempUnit, presUnit);

 // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("datalog.txt", FILE_WRITE);

   // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing data as CSV...");
   //myFile.print("Time: ");
   myFile.print(my_time/1000);
   myFile.print(";");
   myFile.print(temp);
   myFile.print(";");
   myFile.print(hum);
   myFile.print(";");
   myFile.print(pres);
      myFile.print("\n");
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.txt");
  }

   client->print("Time: ");
   client->print(my_time/1000);
   client->print("\t\tTemp: ");
   client->print(temp);
   client->print("°"+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
   client->print("\t\tHumidity: ");
   client->print(hum);
   client->print("% RH");
   client->print("\t\tPressure: ");
   client->print(pres);
   client->println(" Pa");

   delay(1000);
}

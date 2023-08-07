#include <DHT.h>
#include <SDS011.h>
#include <Wire.h>
#include <DS3231.h>
#include <SD.h>
#include <stdio.h>

#define DHTPIN 4      // Pin connected to the DHT sensor (temperature and humidity)
//this change is by Piyush Dahal
#define DHTTYPE DHT22 // DHT sensor type

#define SDS_RX_PIN 2   // Pin connected to the SDS011 RX pin
#define SDS_TX_PIN 3   // Pin connected to the SDS011 TX pin

#define SD_CS_PIN 10  // Chip select pin for SD card module

DS3231 rtc;
DHT dht(DHTPIN, DHTTYPE);
SDS011 sds;

File dataFile;

void setup() {
  Serial.begin(9600);

  rtc.begin();

  dht.begin();

  sds.begin(SDS_RX_PIN, SDS_TX_PIN);
  sds.setWorkingMode(WorkMode::Active); // Set the SDS011 to active mode

  // Initialize the SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card!");
    return;
  }

  // Create a file on the SD card to store data
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Failed to create data file!");
    return;
  }

  // Write the header to the data file
  dataFile.println("Time,Temperature (°C),Humidity (%),PM2.5 (µg/m³)");

  dataFile.close();
}

void loop() {
  DateTime now = rtc.now(); // Get the current time from the RTC module

  // Read temperature and humidity from the DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any DHT sensor reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read DHT sensor data!");
    return;
  }

  // Read PM2.5 air quality data from the SDS011 sensor
  float pm25 = sds.getPM25();

  // Check if the SDS011 reading failed
  if (isnan(pm25)) {
    Serial.println("Failed to read SDS011 sensor data!");
    return;
  }

  // Print the current time, temperature, humidity, and PM2.5 values
  Serial.print("Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print("\tTemperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.print(" %\tPM2.5: ");
  Serial.print(pm25);
  Serial.println(" µg/m³");

  // Append the data to the CSV file on the SD card
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(now.hour(), DEC);
    dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(":");
    dataFile.print(now.second(), DEC);
    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.print(humidity);
    dataFile.print(",");
    dataFile.println(pm25);
    dataFile.close();
  } else {
    Serial.println("Error opening data file!");
  }

  delay(2000);  // Wait for 2 seconds between readings
}

#include <OneWire.h>
#include <DallasTemperature.h>

// Define the pin connected to the DS18B20 data line
const int oneWireBus = 27;

// Setup a one-wire instance to communicate with any OneWire device
OneWire oneWire(oneWireBus);

// Pass our one-wire reference to the Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void setup() {
  // Start the Serial Monitor
  Serial.begin(9600);
  Serial.println("ESP32 DS18B20 Temperature Sensor Test");

  // Start the sensor library
  sensors.begin();
}

void loop() {
  // Tell all devices on the bus to take a temperature reading
  sensors.requestTemperatures();

  // Read the temperature in Celsius
  float tempC = sensors.getTempCByIndex(0);

  // Print the temperature
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  // Wait for 1 second before the next reading
  delay(1000);
}

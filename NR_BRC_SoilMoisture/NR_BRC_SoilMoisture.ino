// Define the ESP32 ADC pin connected to the sensor's A0 pin
const int soilMoisturePin = 12;

// Variable to hold the raw analog reading from the sensor
int sensorValue = 0;

void setup() {
  // Start the Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("ESP32 Soil Moisture Sensor Test");
}

void loop() {
  // Read the analog value from the sensor
  // The value will be between 0 and 4095
  sensorValue = analogRead(soilMoisturePin);

  // Print the raw sensor value to the Serial Monitor
  Serial.print("Soil Moisture Value: ");
  Serial.println(sensorValue);

  // A higher value typically means the soil is dry.
  // A lower value means the soil is wet.
  // You can set a threshold to trigger an action, like watering.
  // if (sensorValue > 2500) {
  //   Serial.println("Soil is dry, time to water!");
  // }

  // Wait for 500 milliseconds before the next reading
  delay(500);
}

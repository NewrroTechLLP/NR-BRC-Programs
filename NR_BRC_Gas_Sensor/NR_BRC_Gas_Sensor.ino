// Define the analog pin connected to the MQ sensor
const int sensorPin = 25;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read the analog value from the sensor
  int sensorValue = analogRead(sensorPin);

  // Print the sensor value to the serial monitor
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  // Add a delay to avoid overwhelming the serial monitor
  delay(500);
}

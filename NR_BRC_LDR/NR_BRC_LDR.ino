// Define the ESP32 ADC pin connected to the LDR's analog output
const int ldrPin = 34;

// Variable to hold the raw analog reading
int ldrValue = 0;

void setup() {
  // Start the Serial Monitor for debugging
  Serial.begin(115200);
  Serial.println("ESP32 LDR Sensor Test");
}

void loop() {
  // Read the analog value from the LDR
  // Value will be between 0 and 4095
  ldrValue = analogRead(ldrPin);

  // Print the raw sensor value to the Serial Monitor
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // A higher value typically means less light, and a lower value means more light.
  // You can adjust the logic based on your voltage divider configuration.

  // Wait for 200 milliseconds before the next reading
  delay(200);
}

// Define the digital pin connected to the rain sensor's D0 pin
const int rainSensorPin = 12;

// Variable to store the sensor's digital state
int rainState = 0;

void setup() {
  // Start the Serial Monitor for debugging
  Serial.begin(115200);
  Serial.println("ESP32 Rain Sensor Test");

  // Set the pin mode for the rain sensor pin
  pinMode(rainSensorPin, INPUT);
}

void loop() {
  // Read the digital value from the rain sensor
  // It's typically LOW when water is detected and HIGH when dry.
  rainState = digitalRead(rainSensorPin);

  // Check the state of the sensor
  if (rainState == LOW) {
    // Print a message when rain is detected
    Serial.println("Rain detected!");
  } else {
    // Print a message when no rain is detected
    Serial.println("No rain.");
  }

  // Wait for 500 milliseconds before the next reading
  delay(500);
}

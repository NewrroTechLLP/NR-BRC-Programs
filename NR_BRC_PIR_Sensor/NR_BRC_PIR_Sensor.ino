// Define the ESP32 digital pin connected to the PIR sensor's OUT pin
const int pirSensorPin = 14;

// Variable to store the sensor's value
int pirState = 0;

void setup() {
  // Start the Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("ESP32 PIR Sensor Test");

  // Set the PIR sensor pin as an input
  pinMode(pirSensorPin, INPUT);
}

void loop() {
  // Read the digital value from the PIR sensor
  pirState = digitalRead(pirSensorPin);

  // Check if the sensor's state is HIGH (motion detected)
  if (pirState == HIGH) {
    // Print a message to the Serial Monitor
    Serial.println("Motion detected!");
  } else {
    // Print a message when no motion is detected
    Serial.println("No motion.");
  }

  // Wait for 200 milliseconds before the next reading
  delay(200);
}

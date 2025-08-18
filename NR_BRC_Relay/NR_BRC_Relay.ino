// Define the digital pin connected to the relay's IN pin
const int relayPin = 14;

void setup() {
  // Start the Serial Monitor for debugging
  Serial.begin(9600);
  Serial.println("ESP32 Relay Module Test");

  // Set the relay pin as an output
  pinMode(relayPin, OUTPUT);
}

void loop() {
  // Turn the relay ON
  // Many relay modules are "active-LOW", meaning a LOW signal turns them on.
  // If your relay is active-HIGH, change LOW to HIGH.
  digitalWrite(relayPin, LOW);
  Serial.println("Relay is ON");

  // Wait for 2 seconds
  delay(2000);

  // Turn the relay OFF
  // A HIGH signal will turn off an active-LOW relay.
  digitalWrite(relayPin, HIGH);
  Serial.println("Relay is OFF");

  // Wait for 2 seconds
  delay(2000);
}

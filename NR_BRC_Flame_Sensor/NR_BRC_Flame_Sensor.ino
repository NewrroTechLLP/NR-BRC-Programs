const int flamePin = 14;  // ADC1 channel 0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int flameValue = analogRead(flamePin);
  Serial.print("Flame sensor value: ");
  Serial.println(flameValue);

  if (flameValue < 1500) {  // Adjust threshold for your sensor
    Serial.println("Flame detected!");
  } else {
    Serial.println("No flame");
  }
  delay(1000);
}

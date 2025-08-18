const int irPin = 27;

void setup() {
  Serial.begin(9600);
  pinMode(irPin, INPUT);
}

void loop() {
  int irState = digitalRead(irPin);
  if (irState == HIGH) {
    Serial.println("Obstacle detected!");
  } else {
    Serial.println("Path clear");
  }
  delay(500);
}

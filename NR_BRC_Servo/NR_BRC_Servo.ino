#include <ESP32Servo.h>

Servo myServo;  // Create servo object

void setup() {
  Serial.begin(115200);

  // Attach servo to pin 33
  myServo.attach(4);

  Serial.println("Servo ready");
}

void loop() {
  // Sweep from 0° to 180°
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);   // Set servo angle
    delay(15);            // Wait for servo to reach position
  }
delay(1000);
  // Sweep back from 180° to 0°
  for (int pos = 180; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
}

// Motor A pins
#define AIN1   14
#define AIN2   27
#define PWMA   12  // PWM pin

// Motor B pins
#define BIN1   26
#define BIN2   25
#define PWMB   13  // PWM pin

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  Serial.println("TB6612FNG Motor Test (analogWrite)");
}

void loop() {
  // Motor A forward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 200); // speed 0-255

  // Motor B forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 200);

  Serial.println("Motors ON");
  delay(3000);

  // Stop motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  analogWrite(PWMA, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 0);

  Serial.println("Motors OFF");
  delay(3000);
}

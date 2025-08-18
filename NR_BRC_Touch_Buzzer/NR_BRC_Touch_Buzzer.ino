#define TOUCH_PIN 35   // Touch sensor pin
#define BUZZER_PIN 32  // Buzzer pin

int buzzerChannel = 0;  
int buzzerFreq = 1000;    // Lower freq (1kHz) often sounds louder for passive buzzers
int buzzerResolution = 8; // 8-bit = values 0–255

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);

  // Setup PWM for buzzer
  ledcSetup(buzzerChannel, buzzerFreq, buzzerResolution);
  ledcAttachPin(BUZZER_PIN, buzzerChannel);
}

void loop() {
  int touchValue = digitalRead(TOUCH_PIN);

  if (touchValue == HIGH) { 
    ledcWrite(buzzerChannel, 255); // Full duty cycle = loudest possible
    Serial.println("Touch Detected - Buzzer ON");
  } else {
    ledcWrite(buzzerChannel, 0);   // OFF
    Serial.println("No Touch - Buzzer OFF");
  }

  delay(100);
}

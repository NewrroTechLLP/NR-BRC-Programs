// Define the pins for the trigger and echo
const int trigPin = 13; // You can change this to any suitable GPIO pin
const int echoPin = 12; // You can change this to any suitable GPIO pin

// Define variables for duration and distance
long duration;
int distanceCm;

void setup() {
  // Start serial communication at a baud rate of 115200
  Serial.begin(9600);

  // Define the pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Clear the trigPin to ensure a clean HIGH pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10 microsecond HIGH pulse to the trigPin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the HIGH pulse on the echoPin
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance based on the speed of sound
  distanceCm = duration * 0.0343 / 2;

  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // Wait for a short time before the next measurement
  delay(500);
}

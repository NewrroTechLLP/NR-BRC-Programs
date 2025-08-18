#include <SPI.h>

#include <MFRC522.h>

// Define the pins for the RFID reader
#define SS_PIN 5  // SDA (Slave Select) pin
#define RST_PIN 2 // RST (Reset) pin

// Create an instance of the MFRC522 class
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  // Start serial communication at a baud rate of 115200
  Serial.begin(115200);

  // Initialize the SPI bus
  SPI.begin();

  // Initialize the MFRC522 RFID reader
  rfid.PCD_Init();

  delay(2000);

  Serial.println("Scan an RFID tag or card...");
}

void loop() {
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print the UID to the Serial Monitor
  Serial.print("UID tag: ");
  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString += (rfid.uid.uidByte[i] < 0x10 ? " 0" : "");
    uidString += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println(uidString);

  // Halt the communication with the card
  rfid.PICC_HaltA();
  delay(2000);
}

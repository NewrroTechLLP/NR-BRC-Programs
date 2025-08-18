#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Animation Settings ---
String textToType = "NEWRRO";
int typingDelay = 150; // Milliseconds between each character

void setup() {
  Serial.begin(115200);

  // Initialize the OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // --- Start Typewriter Animation ---

  // Set text properties
  display.setTextSize(3);      // A larger text size
  display.setTextColor(SSD1306_WHITE); // White text

  // Loop through each character of the string
  for (int i = 0; i < textToType.length(); i++) {
    // 1. Clear the display before drawing the new frame
    display.clearDisplay();
    
    // 2. Set the cursor position to center the text
    // We'll calculate the position based on the final text to keep it stationary
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(textToType, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);

    // 3. Get the substring to print (from the beginning to the current character)
    display.print(textToType.substring(0, i + 1));
    
    // 4. Push the buffer to the display
    display.display();
    
    // 5. Wait for a moment before the next character
    delay(typingDelay);
  }

  // Animation is complete
  Serial.println("Animation finished.");
}

void loop() {
  // The animation runs once in setup(), so the loop is empty.
  // The final text will remain on the screen.
}

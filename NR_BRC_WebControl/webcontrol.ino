#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <qrcodegen.hpp>

using namespace qrcodegen;

#define TCA9548A_ADDR 0x70
#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SDA 21
#define I2C_SCL 22

Adafruit_SH1106G display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SH1106G display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SH1106G display3(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RED1 4
#define YELLOW1 2
#define GREEN1 15
#define RED2 5
#define YELLOW2 17
#define GREEN2 16
#define TRIG1 13
#define ECHO1 12
#define TRIG2 26
#define ECHO2 25
#define SERVO1_PIN 14
#define SERVO2_PIN 27
#define BUZZER 33

bool radar1Enabled = false, radar2Enabled = false;
float distance1 = 0, distance2 = 0;
Servo servo1, servo2;
AsyncWebServer server(80);

const char* ssid = "ESP32_Hotspot";
const char* password = "NewrroTech1";

int animFrame[3] = {0,0,0}, currentSymbol[3] = {0,0,0};
bool blinkState[3] = {false, false, false};
unsigned long lastAnimUpdate = 0;

String qrData[3] = {"","",""}; // QR data for 3 displays
bool showQR[3] = {false, false, false}; // flag to show QR on display

void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void displayNewrro(Adafruit_SH1106G &disp) {
  disp.clearDisplay();
  disp.setTextSize(3);
  disp.setTextColor(SH110X_WHITE);
  int16_t x1, y1; uint16_t w, h;
  disp.getTextBounds("Newrro", 0, 0, &x1, &y1, &w, &h);
  disp.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2);
  disp.print("Newrro");
  disp.display();
}

void drawQRCode(Adafruit_SH1106G &disp, String text) {
  // Check if text is empty
  if (text.length() == 0) {
    disp.clearDisplay();
    disp.setTextSize(1);
    disp.setTextColor(SH110X_WHITE);
    disp.setCursor(0, 0);
    disp.println("No QR data");
    disp.display();
    return;
  }
  
  disp.clearDisplay();
  
  try {
    // Generate QR code using qrcodegen library with lower error correction for smaller QR
    QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::ECC_LOW);
    
    int size = qr.getSize();
    Serial.print("QR Code size: ");
    Serial.println(size);
    Serial.print("QR Data: ");
    Serial.println(text);
    
    // Calculate maximum possible scale to fill the screen
    // We want to use as much screen space as possible
    int scaleWidth = SCREEN_WIDTH / size;
    int scaleHeight = SCREEN_HEIGHT / size;
    int scale = min(scaleWidth, scaleHeight);
    
    // Ensure minimum scale of 1 (but prefer larger)
    if (scale < 1) {
      scale = 1;
      Serial.println("Warning: QR code may be too complex for optimal display");
    }
    
    // Calculate actual QR dimensions
    int qrPixelWidth = size * scale;
    int qrPixelHeight = size * scale;
    
    // Center the QR code on screen
    int offsetX = (SCREEN_WIDTH - qrPixelWidth) / 2;
    int offsetY = (SCREEN_HEIGHT - qrPixelHeight) / 2;
    
    // If QR code is smaller than screen, try to increase scale
    if (qrPixelWidth < SCREEN_WIDTH && qrPixelHeight < SCREEN_HEIGHT) {
      // Try one size larger if it fits
      int testScale = scale + 1;
      int testWidth = size * testScale;
      int testHeight = size * testScale;
      
      if (testWidth <= SCREEN_WIDTH && testHeight <= SCREEN_HEIGHT) {
        scale = testScale;
        qrPixelWidth = testWidth;
        qrPixelHeight = testHeight;
        offsetX = (SCREEN_WIDTH - qrPixelWidth) / 2;
        offsetY = (SCREEN_HEIGHT - qrPixelHeight) / 2;
      }
    }
    
    Serial.print("Final Scale: ");
    Serial.print(scale);
    Serial.print(", QR Size: ");
    Serial.print(qrPixelWidth);
    Serial.print("x");
    Serial.print(qrPixelHeight);
    Serial.print(", Screen: ");
    Serial.print(SCREEN_WIDTH);
    Serial.print("x");
    Serial.print(SCREEN_HEIGHT);
    Serial.print(", Offset: (");
    Serial.print(offsetX);
    Serial.print(", ");
    Serial.print(offsetY);
    Serial.println(")");
    
    // Clear the entire display
    disp.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SH110X_BLACK);
    
    // Draw QR code modules
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        if (qr.getModule(x, y)) {
          // Calculate pixel position
          int pixelX = offsetX + x * scale;
          int pixelY = offsetY + y * scale;
          
          // Draw filled rectangle for each black module
          // Make sure we don't draw outside screen bounds
          if (pixelX >= 0 && pixelY >= 0 && 
              pixelX + scale <= SCREEN_WIDTH && 
              pixelY + scale <= SCREEN_HEIGHT) {
            disp.fillRect(pixelX, pixelY, scale, scale, SH110X_WHITE);
          }
        }
      }
    }
    
    Serial.println("QR Code drawn successfully at maximum size");
    
  } catch (const std::exception& e) {
    Serial.print("QR generation error: ");
    Serial.println(e.what());
    
    disp.setTextSize(1);
    disp.setCursor(0, 0);
    disp.println("QR Error:");
    disp.println("Text may be too long");
    disp.println("or contain invalid");
    disp.println("characters");
    
  } catch (...) {
    Serial.println("Unknown QR generation error");
    
    disp.setTextSize(1);
    disp.setCursor(0, 0);
    disp.println("QR generation failed");
    disp.println("Unknown error");
  }
  
  disp.display();
}

// Test function to verify QR generation
void testQRCode() {
  Serial.println("Testing QR Code generation...");
  
  try {
    QrCode testQr = QrCode::encodeText("TEST", QrCode::Ecc::ECC_LOW);
    Serial.println("Basic QR generation: SUCCESS");
    
    // Test with URL
    QrCode urlQr = QrCode::encodeText("https://google.com", QrCode::Ecc::ECC_LOW);
    Serial.println("URL QR generation: SUCCESS");
    
  } catch (...) {
    Serial.println("QR generation test: FAILED");
  }
}

void updateDisplay(int idx) {
  Adafruit_SH1106G* disp; uint8_t ch;
  switch(idx) {
    case 0: disp=&display1; ch=1; break;
    case 1: disp=&display2; ch=2; break;
    case 2: disp=&display3; ch=3; break;
    default: return;
  }
  tcaSelect(ch);

  if(showQR[idx]){
    drawQRCode(*disp, qrData[idx]);
    return;
  }

  disp->clearDisplay();
  disp->setTextColor(SH110X_WHITE);

  if (currentSymbol[idx] == 0) { displayNewrro(*disp); return; }

  switch(currentSymbol[idx]) {
    case 1: { // Right Arrow - Much Larger
      int x = (animFrame[idx] * 10) % (SCREEN_WIDTH + 80) - 50; // Much faster animation
      
      // Large arrow head (triangle)
      disp->fillTriangle(x + 45, 32,   // tip point
                        x + 5, 15,    // top back point  
                        x + 5, 49,    // bottom back point
                        SH110X_WHITE);
      
      // Longer thick arrow shaft (rectangle)
      disp->fillRect(x - 55, 25, 60, 14, SH110X_WHITE);
      
      // Additional triangle for more solid look
      disp->fillTriangle(x + 40, 32,   
                        x + 10, 18,    
                        x + 10, 46,    
                        SH110X_WHITE);
      break;
    }
    
    case 2: { // Left Arrow - Much Larger
      int x = SCREEN_WIDTH + 25 - ((animFrame[idx] * 10) % (SCREEN_WIDTH + 80)); // Much faster animation
      
      // Large arrow head (triangle) pointing left
      disp->fillTriangle(x - 45, 32,   // tip point
                        x - 5, 15,    // top back point
                        x - 5, 49,    // bottom back point
                        SH110X_WHITE);
      
      // Longer thick arrow shaft (rectangle)
      disp->fillRect(x - 5, 25, 60, 14, SH110X_WHITE);
      
      // Additional triangle for more solid look
      disp->fillTriangle(x - 40, 32,   
                        x - 10, 18,   
                        x - 10, 46,   
                        SH110X_WHITE);
      break;
    }
    
    case 3: { // Caution - Larger Triangle
      // Outer triangle (larger)
      disp->drawTriangle(64, 2,      // top point
                        15, 58,     // bottom left
                        113, 58,    // bottom right
                        SH110X_WHITE);
      
      // Inner triangle for double border effect
      disp->drawTriangle(64, 4,      
                        17, 56,     
                        111, 56,    
                        SH110X_WHITE);
      
      // Third triangle for thick border
      disp->drawTriangle(64, 6,      
                        19, 54,     
                        109, 54,    
                        SH110X_WHITE);
      
      if(blinkState[idx]){
        // Larger exclamation mark
        disp->setTextSize(4);  // Increased from 3
        int16_t x1,y1; uint16_t w,h;
        disp->getTextBounds("!",0,0,&x1,&y1,&w,&h);
        disp->setCursor((SCREEN_WIDTH-w)/2, 18); // Adjusted position
        disp->print("!");
      }
      break;
    }
    
    case 4: { // Stop Sign - Larger
      // Outer rectangle (larger)
      disp->drawRect(5, 5, 118, 54, SH110X_WHITE);   // Increased size
      disp->drawRect(6, 6, 116, 52, SH110X_WHITE);   // Double border
      disp->drawRect(7, 7, 114, 50, SH110X_WHITE);   // Triple border for thickness
      
      if(blinkState[idx]){
        disp->setTextSize(3);  // Kept same size as it fits well
        int16_t x1,y1; uint16_t w,h;
        disp->getTextBounds("STOP",0,0,&x1,&y1,&w,&h);
        disp->setCursor((SCREEN_WIDTH-w)/2, 26); 
        disp->print("STOP");
      }
      break;
    }
  }
  disp->display();
}

float readUltrasonic(int trig, int echo){
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long dur=pulseIn(echo,HIGH,20000);
  return dur*0.034/2;
}

void checkRadar(){
  bool buzz=false;
  if(radar1Enabled && (distance1=readUltrasonic(TRIG1,ECHO1))<7 && distance1>0) buzz=true;
  if(radar2Enabled && (distance2=readUltrasonic(TRIG2,ECHO2))<7 && distance2>0) buzz=true;
  digitalWrite(BUZZER, buzz);
}

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>NEWRRO TRAFFIC HUB</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    html, body {
      height: 100%;
      background: #fff;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    }
    .container {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 20px 15px;
    }
    h1 {
      font-size: 4rem;
      text-align: center;
      background: linear-gradient(90deg, #ef5df6, #785af6);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      margin-bottom: 20px;
      line-height: 1.2;
    }
    h2 {
      color: #555;
      font-size: 1.4rem;
      margin: 18px 0 10px 0;
      text-align: center;
    }
    .control-row, .traffic-lights, .servo-controls, .oled-controls, .qr-controls {
      display: flex;
      justify-content: center;
      flex-wrap: wrap;
      gap: 10px;
      margin-bottom: 15px;
      width: 100%;
      max-width: 600px;
    }
    button, select {
      padding: 18px 18px;       /* Taller, with reduced side padding */
      font-size: 1.4rem;
      background: linear-gradient(90deg, #ef5df6, #785af6);
      color: white;
      border: none;
      border-radius: 25px;
      cursor: pointer;
      transition: all 0.3s ease;
      min-width: 140px;
      max-width: 200px;
      text-align: center;
      font-weight: 600;
    }
    button:hover, select:hover {
      transform: scale(1.05);
      box-shadow: 0 0 15px #b45eff;
    }
    input[type="text"] {
      width: 100%;
      max-width: 400px;
      padding: 14px 15px;
      font-size: 1.1rem;
      border: 2px solid #ddd;
      border-radius: 25px;
      outline: none;
      transition: border-color 0.3s ease;
    }
    input[type="text"]:focus {
      border-color: #ef5df6;
    }
    .qr-section {
      width: 100%;
      max-width: 600px;
      margin-top: 20px;
    }
    .qr-input-group {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 12px;
    }

    /* Mobile adjustments */
    @media (max-width: 480px) {
      h1 { font-size: 2.4rem; }
      h2 { font-size: 1.2rem; }
      button, select {
        padding: 14px 8px;
        font-size: 1.1rem;
        min-width: 120px;
        max-width: 160px;
      }
      input[type="text"] {
        font-size: 1rem;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>NEWRRO TRAFFIC<br>HUB</h1>

    <h2>Traffic Light 1</h2>
    <div class="traffic-lights">
      <button onclick="setLight('RED1')">Red</button>
      <button onclick="setLight('YELLOW1')">Yellow</button>
      <button onclick="setLight('GREEN1')">Green</button>
    </div>

    <h2>Traffic Light 2</h2>
    <div class="traffic-lights">
      <button onclick="setLight('RED2')">Red</button>
      <button onclick="setLight('YELLOW2')">Yellow</button>
      <button onclick="setLight('GREEN2')">Green</button>
    </div>

    <h2>Servo Control</h2>
    <div class="servo-controls">
      <button onclick="controlServo(1,0)">Servo1 Close</button>
      <button onclick="controlServo(1,90)">Servo1 Open</button>
    </div>
    <div class="servo-controls">
      <button onclick="controlServo(2,0)">Servo2 Close</button>
      <button onclick="controlServo(2,90)">Servo2 Open</button>
    </div>

    <h2>Radar Control</h2>
    <div class="control-row">
      <button onclick="setRadar(1,1)">Radar 1 ON</button>
      <button onclick="setRadar(1,0)">Radar 1 OFF</button>
    </div>
    <div class="control-row">
      <button onclick="setRadar(2,1)">Radar 2 ON</button>
      <button onclick="setRadar(2,0)">Radar 2 OFF</button>
    </div>

    <h2>OLED Display</h2>
    <div class="oled-controls">
      <select id="display">
        <option value="1">Display 1</option>
        <option value="2">Display 2</option>
        <option value="3">Display 3</option>
      </select>
      <select id="symbol">
        <option value="1">Right Arrow</option>
        <option value="2">Left Arrow</option>
        <option value="3">Caution</option>
        <option value="4">Stop</option>
      </select>
    </div>
    <div class="control-row">
      <button onclick="controlOLED()">Show Symbol</button>
    </div>

    <div class="qr-section">
      <h2>QR Code Control</h2>
      <div class="qr-input-group">
        <select id="qrdisp">
          <option value="1">Display 1</option>
          <option value="2">Display 2</option>
          <option value="3">Display 3</option>
        </select>
        <input type="text" id="qrtext" placeholder="Enter QR code content">
        <div class="qr-controls">
          <button onclick="sendQR()">Show QR Code</button>
          <button onclick="clearQR()">Clear QR</button>
        </div>
      </div>
    </div>
  </div>

  <script>
    function setLight(c){ fetch(`/light?color=${c}`); }
    function controlServo(id, angle){ fetch(`/servo?id=${id}&angle=${angle}`); }
    function setRadar(id, state){ fetch(`/radar?r${id}=${state}`); }
    function controlOLED(){
      let d = document.getElementById('display').value;
      let s = document.getElementById('symbol').value;
      fetch(`/oled?d=${d}&s=${s}`);
    }
    function sendQR(){
      let txt = document.getElementById('qrtext').value;
      let d = document.getElementById('qrdisp').value;
      if(txt.trim() === '') return alert('Please enter QR code content');
      fetch(`/qr?d=${d}&data=${encodeURIComponent(txt)}`);
    }
    function clearQR(){
      let d = document.getElementById('qrdisp').value;
      fetch(`/qr?d=${d}&data=`);
      fetch(`/oled?d=${d}&s=0`);
    }
  </script>
</body>
</html>
)rawliteral";

void setup(){
  Serial.begin(115200); 
  Wire.begin(I2C_SDA,I2C_SCL);
  
  // Test QR Code generation
  testQRCode();
  
  // Initialize pins
  pinMode(RED1,OUTPUT); pinMode(YELLOW1,OUTPUT); pinMode(GREEN1,OUTPUT);
  pinMode(RED2,OUTPUT); pinMode(YELLOW2,OUTPUT); pinMode(GREEN2,OUTPUT);
  pinMode(TRIG1,OUTPUT); pinMode(ECHO1,INPUT);
  pinMode(TRIG2,OUTPUT); pinMode(ECHO2,INPUT);
  pinMode(BUZZER,OUTPUT);
  
  // Initialize servos
  servo1.attach(SERVO1_PIN); servo2.attach(SERVO2_PIN);
  servo1.write(0); servo2.write(0);
  
  // Initialize WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Initialize displays
  tcaSelect(1); 
  if(display1.begin(OLED_ADDR,true)) {
    displayNewrro(display1);
    Serial.println("Display 1 initialized");
  }
  
  tcaSelect(2); 
  if(display2.begin(OLED_ADDR,true)) {
    displayNewrro(display2);
    Serial.println("Display 2 initialized");
  }
  
  tcaSelect(3); 
  if(display3.begin(OLED_ADDR,true)) {
    displayNewrro(display3);
    Serial.println("Display 3 initialized");
  }

  // Web server routes
  server.on("/",HTTP_GET,[](AsyncWebServerRequest *r){
    r->send_P(200,"text/html",webpage);
  });
  
  server.on("/oled",HTTP_GET,[](AsyncWebServerRequest *r){
    int d=r->getParam("d")->value().toInt()-1;
    int s=r->getParam("s")->value().toInt();
    if(d>=0 && d<3){ 
      currentSymbol[d]=s; 
      showQR[d]=false; 
      animFrame[d]=0; 
      blinkState[d]=true; 
      Serial.println("OLED symbol set: Display " + String(d+1) + ", Symbol " + String(s));
    }
    r->send(200,"text/plain","OK");
  });
  
  server.on("/qr",HTTP_GET,[](AsyncWebServerRequest *r){
    int d=r->getParam("d")->value().toInt()-1;
    String data=r->getParam("data")->value();
    if(d>=0 && d<3){ 
      qrData[d]=data; 
      showQR[d]=(data.length() > 0); 
      Serial.println("QR Code set: Display " + String(d+1) + ", Data: " + data);
    }
    r->send(200,"text/plain","QR Set");
  });
  
  server.on("/light",HTTP_GET,[](AsyncWebServerRequest *r){
    String c=r->getParam("color")->value();
    // Turn off all lights first
    digitalWrite(RED1,LOW); digitalWrite(YELLOW1,LOW); digitalWrite(GREEN1,LOW);
    digitalWrite(RED2,LOW); digitalWrite(YELLOW2,LOW); digitalWrite(GREEN2,LOW);
    // Turn on selected light
    digitalWrite(RED1,c=="RED1"); digitalWrite(YELLOW1,c=="YELLOW1"); digitalWrite(GREEN1,c=="GREEN1");
    digitalWrite(RED2,c=="RED2"); digitalWrite(YELLOW2,c=="YELLOW2"); digitalWrite(GREEN2,c=="GREEN2");
    Serial.println("Light set: " + c);
    r->send(200,"text/plain","OK");
  });
  
  server.on("/servo",HTTP_GET,[](AsyncWebServerRequest *r){
    int id=r->getParam("id")->value().toInt();
    int a=r->getParam("angle")->value().toInt();
    if(id==1) servo1.write(a); 
    else if(id==2) servo2.write(a);
    Serial.println("Servo " + String(id) + " set to " + String(a) + " degrees");
    r->send(200,"text/plain","OK");
  });
  
  server.on("/radar",HTTP_GET,[](AsyncWebServerRequest *r){
    if(r->hasParam("r1")) {
      radar1Enabled=r->getParam("r1")->value()=="1";
      Serial.println("Radar 1: " + String(radar1Enabled ? "ON" : "OFF"));
    }
    if(r->hasParam("r2")) {
      radar2Enabled=r->getParam("r2")->value()=="1";
      Serial.println("Radar 2: " + String(radar2Enabled ? "ON" : "OFF"));
    }
    r->send(200,"text/plain","OK");
  });
  
  server.begin();
  Serial.println("Web server started");
}

void loop(){
  checkRadar();
  
  unsigned long now=millis();
  if(now-lastAnimUpdate>800){  // Changed from 200ms to 800ms for slower blinking
    lastAnimUpdate=now;
    for(int i=0;i<3;i++){
      if(!showQR[i]){
        animFrame[i]++;
        if(currentSymbol[i]>=3) blinkState[i]=!blinkState[i];
      }
      updateDisplay(i);
    }
  }
}
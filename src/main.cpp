#ifdef ENABLE_OLED
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define MAX_LINES 8        // 64px / 8px per line
#define MAX_LINE_LEN 21    // ~21 chars per line at text size 1

#define SDA_PIN 4   // GPIO4 (D2)
#define SCL_PIN 5   // GPIO5 (D1)

// On-board LED (ESP-12F / ESP8266)
#define LED_PIN 2   // GPIO2 (ACTIVE LOW)

String lines[MAX_LINES];
String inputLine = "";
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void showOnOLED(const String &text);
void heartbeat();
void redraw();
void addLine(const String &newLine);

void setup() {
  // On-board LED setup
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED OFF (active LOW)
  Serial.begin(9600);


  // OLED I2C init
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("initialised i2c");

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    // Rapid blink = OLED failure
    while (true) {
      Serial.println("not able to connect oled");
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
    }
  }

  // Force OLED ON
  display.ssd1306_command(SSD1306_DISPLAYON);

  // Static content
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 20);
  display.print("READY");
  display.display();
}

void loop() {
  // Heartbeat blink = board running
  // digitalWrite(LED_PIN, LOW);   // LED ON
  // delay(10);
  // digitalWrite(LED_PIN, HIGH);  // LED OFF
  // delay(5000);
  heartbeat();  


  #ifdef ENABLE_SERIAL_IP
    // Read Serial input

    while (Serial.available()) {
      char c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (inputLine.length() > 0) {
          addLine(inputLine);
          inputLine = "";
        }
      } else {
        inputLine += c;
      }
      showOnOLED(inputLine);
    display.ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);

    }

  #endif

}

void showOnOLED(const String &text) {
  // display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.println(text);  // simple single-line display
  display.display();

  Serial.print("Displayed: ");
  Serial.println(text);
}

void heartbeat() {
  static unsigned long last = 0;
  if (millis() - last >= 500) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    last = millis();
  }
}


void addLine(const String &newLine) {
  // Shift lines up
  for (int i = 0; i < MAX_LINES - 1; i++) {
    lines[i] = lines[i + 1];
  }

  // Trim line length
  lines[MAX_LINES - 1] = newLine.substring(0, MAX_LINE_LEN);

  redraw();
}

void redraw() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  for (int i = 0; i < MAX_LINES; i++) {
    display.println(lines[i]);
  }

  display.display();
}
#else
#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 4   // GPIO4 (D2)
#define SCL_PIN 5   // GPIO5 (D1)

void setup() {
  Serial.begin(9600);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println();
  Serial.println("I2C Scanner Started");
}

void loop() {
  byte error, address;
  int deviceCount = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.print("Total devices found: ");
    Serial.println(deviceCount);
  }

  Serial.println("Scan complete\n");
  delay(5000); // scan every 5 seconds
}
#endif
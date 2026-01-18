// https://wokwi.com/projects/453329992388004865
//To be ran on Arduino IDE
#include <Wire.h>
//Make sure necessary packages are downloaded
#include <Adafruit_SSD1306.h>
#include "smile_animations.h"
#include "ota_update.h"
//Make Sure Address is Correct

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // initialization for the used OLED display

int frame=0;


void setup(void)
{
  u8g2.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("\nStarting ESP32 OTA Update");

  connectToWiFi();
  Serial.println("Device is ready.");
  Serial.println("Current Firmware Version: " + String(currentFirmwareVersion));
  checkForFirmwareUpdate();
}

void loop(void)
{
  // Serial.println("Current Firmware Version: " + String(currentFirmwareVersion));
	// frame++;
  run_first_smile();
  run_home_page();
  run_latest_anime();
  delay(100);
  u8g2.clearBuffer();

}
void run_first_smile(void)
{
  for(frame=0;frame <= MAX_FRAME;frame++)
  {
    u8g2.clearBuffer();
    u8g2.drawXBMP(0,0,128,64, first_smile[frame]);
    u8g2.sendBuffer();
    delay(1);
  }
  

}
void run_latest_anime(void)
{
  for(frame=0;frame <= MAX_FRAME_LATEST;frame++)
  {
  u8g2.clearBuffer();
  u8g2.drawXBMP(0,0,128,64, latest_anime_allArray[frame]);
  u8g2.sendBuffer();
  delay(1);

  }
}

void run_home_page(void)
{
  for(frame=0;frame <= 24;frame++)
  {
  u8g2.clearBuffer();
  u8g2.drawXBMP(0,0,128,64, home_pageallArray[frame]);
  u8g2.sendBuffer();
  delay(1);
  }
}
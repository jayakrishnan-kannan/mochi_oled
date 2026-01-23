#include "angry.h"
#include <U8g2lib.h>
#include <Adafruit_SSD1306.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // initialization for the used OLED display

void playGIF(const AnimatedGIF* gif) {
    for (uint8_t frame_index = 0; frame_index < gif->frame_count; frame_index++)
    {
                u8g2.clearBuffer();
                
                // Display the current frame
                // for (uint16_t y = 0; y < gif->height; y++) {
                //     for (uint16_t x = 0; x < gif->width; x++) {
                //         uint16_t byteIndex = y * (((gif->width + 7) / 8)) + (x / 8);
                //         uint8_t bitIndex = 7 - (x % 8);
                //         if (gif->frames[frame][byteIndex] & (1 << bitIndex)) {
                //             u8g2.drawPixel(x, y);
                //         }
                //     }
                // }
                 u8g2.drawXBMP(0,0,128,64, gif->frames[frame_index]);
                
                u8g2.sendBuffer();
                delay(gif->delays[frame_index]);
            }
          }
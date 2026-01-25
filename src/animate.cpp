#include "animate.h"
#include <U8g2lib.h>
#include <Adafruit_SSD1306.h>
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // initialization for the used OLED display

uint8_t framebuffer[1024];

void playGIF(const expression *expression) {
   for (uint8_t frame = 0; frame < FRAME_COUNT; frame++) {

    if (frame == 0) {
      memcpy_P(framebuffer, expression->base_frame, 1024);
    } else {
      apply_deltas(framebuffer,
                   expression->delta_frames[frame],
                   expression->delta_counts[frame]);
    }

    u8g2.clearBuffer();
    u8g2.drawXBMP(0, 0, 128, 64, framebuffer);
    u8g2.sendBuffer();

    delay(50);
  }
}

void apply_deltas(uint8_t* fb, const delta_t* deltas, uint16_t count)
{
  // bool page_dirty[8] = {false};

  // 1. Detect dirty pages
  // for (uint16_t i = 0; i < count; i++) {
  //   delta_t d;
  //   memcpy_P(&d, &deltas[i], sizeof(delta_t));
  //   uint8_t page = d.index >> 7;   // index / 128
  //   page_dirty[page] = true;
  // }

  // // 2. Clear dirty pages
  // for (uint8_t page = 0; page < 8; page++) {
  //   if (page_dirty[page]) {
  //     memset(&fb[page * 128], 0x00, 128);
  //   }
  // }

  // 3. Apply deltas
  for (uint16_t i = 0; i < count; i++) {
    uint16_t index = pgm_read_word(&deltas[i].index);
    uint8_t value  = pgm_read_byte(&deltas[i].value);
    fb[index] = value;  
    }
}
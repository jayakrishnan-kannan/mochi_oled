#include <Arduino.h>
#include <U8g2lib.h>
#include "animate.h"   // generated file

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);


expression expressions_list[] = {
  embarrassed_expression,
  excited_2_expression,
  frustrated_expression,
  happy_expression,
  happy_2_expression,
  laugh_expression,
  love_expression,
  proud_expression,
  relaxed_expression,
  sleepy_expression,
  sleepy_3_expression,
  angry_2_expression,
  angry_expression,
  confused_2_expression,
  content_expression,
  determined_expression
};
void setup() {
  u8g2.begin();

  // memcpy_P(framebuffer, angry_frames, 1024);
}

void loop() {
  playGIF(&expressions_list[random(0, sizeof(expressions_list)/sizeof(expressions_list[0]))]);
  delay(1000);
}


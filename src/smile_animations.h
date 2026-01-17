_Pragma("once")
#include <U8g2lib.h>

#define MAX_FRAME 34
extern const unsigned char* first_smile[MAX_FRAME];
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2; // initialization for the used OLED display
_Pragma("once")
#include <U8g2lib.h>

#define MAX_FRAME 34
#define MAX_FRAME_LATEST 133
extern const unsigned char* first_smile[MAX_FRAME];
extern const unsigned char* latest_anime_allArray[MAX_FRAME_LATEST];
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2; // initialization for the used OLED display
void run_latest_anime(void);
void run_first_smile(void);
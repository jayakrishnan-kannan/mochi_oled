#include <stdint.h>
#include <pgmspace.h>
#include <U8g2lib.h>

// Define data structures for GIF
#define FRAME_COUNT 75
#ifndef ANIMATED_GIF_DEFINED
#define ANIMATED_GIF_DEFINED
typedef struct {
    const uint8_t frame_count;      // Number of frames
    const uint16_t width;           // GIF width
    const uint16_t height;          // GIF height
    const uint16_t* delays;         // Pointer to the delay array
    const uint8_t (* frames)[1024]; // Pointer to the frame data array
} AnimatedGIF;

typedef struct {
  const uint16_t index;
  const uint8_t value;
} delta_t;


typedef struct {
    const uint8_t* base_frame;          // Pointer to the base frame data
    const delta_t** delta_frames;       // Pointer to array of pointers to delta frames
    const uint16_t* delta_counts;       // Pointer to array of delta counts per frame
} expression;

extern const expression proud_expression;
extern const expression determined_expression;

extern const expression angry_expression;
extern const expression happy_expression;
extern const expression embarrassed_expression;
extern const expression excited_2_expression;
extern const expression frustrated_expression;
extern const expression happy_expression;
extern const expression happy_2_expression;
extern const expression laugh_expression;
extern const expression love_expression;
extern const expression proud_expression;
extern const expression relaxed_expression;
extern const expression sleepy_expression;
extern const expression sleepy_3_expression;
extern const expression confused_2_expression;
extern const expression content_expression;
extern const expression angry_expression;
extern const expression angry_2_expression;

 
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2; // initialization for the used OLED display
void playGIF(const expression *expression);
void apply_deltas(uint8_t* fb, const delta_t* deltas, uint16_t count);
extern const AnimatedGIF giphy_gif;
extern const AnimatedGIF angry_gif;
extern const AnimatedGIF happy_gif;
extern const AnimatedGIF first_smile_gif;


#endif 
#include <stdint.h>
#include <pgmspace.h>

// Define data structures for GIF
#ifndef ANIMATED_GIF_DEFINED
#define ANIMATED_GIF_DEFINED
typedef struct {
    const uint8_t frame_count;      // Number of frames
    const uint16_t width;           // GIF width
    const uint16_t height;          // GIF height
    const uint16_t* delays;         // Pointer to the delay array
    const uint8_t (* frames)[1024]; // Pointer to the frame data array
} AnimatedGIF;

void playGIF(const AnimatedGIF* gif);
extern const AnimatedGIF angry_gif;


#endif 
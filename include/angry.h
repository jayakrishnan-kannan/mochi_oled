#ifndef ANGRY_H
        #define ANGRY_H
#include "animate.h"
// ANIMATED_GIF_DEFINED
        //General function to display GIF
        /*
        void playGIF(const AnimatedGIF* gif) {
            for (uint8_t frame = 0; frame < gif->frame_count; frame++) {
                display.clearDisplay();
                
                // Display the current frame
                for (uint16_t y = 0; y < gif->height; y++) {
                    for (uint16_t x = 0; x < gif->width; x++) {
                        uint16_t byteIndex = y * (((gif->width + 7) / 8)) + (x / 8);
                        uint8_t bitIndex = 7 - (x % 8);
                        if (gif->frames[frame][byteIndex] & (1 << bitIndex)) {
                            display.drawPixel(x, y, WHITE);
                        }
                    }
                }
                
                display.display();
                delay(gif->delays[frame]);
            }
        }
            */
        // Define a constant for angry
#define ANGRY_FRAME_COUNT 75
#define ANGRY_WIDTH 128
#define ANGRY_HEIGHT 64
// Define data for angry


// In the setup function or loop, you can call:
// playGIF(&angry_gif);

#endif

#include <stdint.h>

#define SRC_DIM 29
#define DEST_DIM 145
#define SCALE 5

void upscale_exact(uint8_t src[SRC_DIM][SRC_DIM], uint8_t dest[DEST_DIM][DEST_DIM]) {
    // Loop through every pixel in the source (29x29)
    for (int y = 0; y < SRC_DIM; y++) {
        for (int x = 0; x < SRC_DIM; x++) {
            
            uint8_t pixel_val = src[y][x];

            // Fill the corresponding 5x5 block in the destination
            for (int dy = 0; dy < SCALE; dy++) {
                for (int dx = 0; dx < SCALE; dx++) {
                    
                    // Calculate exact position in the 145x145 grid
                    int dest_y = (y * SCALE) + dy;
                    int dest_x = (x * SCALE) + dx;
                    
                    dest[dest_y][dest_x] = pixel_val;
                }
            }
        }
    }
}
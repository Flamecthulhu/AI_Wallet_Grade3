#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Source Data (29x29 packed into 116 bytes)
const unsigned char version3[116] = {
    0XFE,0XA4,0X3B,0XF8,0X82,0X53,0XBA,0X08,0XBA,0X25,0X7A,0XE8,0XBA,0XC3,0XD2,0XE8,
    0XBA,0XF1,0X2A,0XE8,0X82,0X26,0X9A,0X08,0XFE,0XAA,0XAB,0XF8,0X00,0X81,0X08,0X00,
    0XEF,0XFF,0X4E,0X20,0X0C,0XD9,0X25,0X68,0X8B,0XDC,0X1E,0XF0,0X95,0X21,0X58,0X50,
    0X88,0X7E,0XAE,0X38,0XE1,0X55,0X50,0X40,0X0C,0XAB,0X63,0XA0,0X54,0X63,0X11,0X70,
    0XE6,0XFC,0X83,0X78,0X53,0X17,0X7D,0X78,0XCD,0XA5,0X72,0XB0,0X55,0X5C,0X85,0XE0,
    0X1F,0XA0,0X8F,0X20,0X48,0X95,0X68,0X00,0XCA,0XA7,0XAB,0XF8,0X58,0XE1,0X12,0X08,
    0XEF,0XD3,0X72,0XE8,0X10,0XEF,0X92,0XE8,0XCE,0X3D,0X5A,0XE8,0X54,0XD6,0X32,0X08,
    0XE8,0XBB,0XD3,0XF8
};

// Dimensions
#define SRC_DIM 29
#define DEST_DIM 145
#define SCALE 5
#define BYTES_PER_ROW 4 // ceil(29 / 8)

// Output buffer (0=Black, 255=White)
uint8_t dest_image[DEST_DIM][DEST_DIM];

void unpack_and_upscale() {
    for (int y = 0; y < SRC_DIM; y++) {
        for (int x = 0; x < SRC_DIM; x++) {
            
            // 1. Calculate source bit location
            int byte_index = (y * BYTES_PER_ROW) + (x / 8);
            int bit_pos    = 7 - (x % 8); // MSB first
            
            // 2. Extract bit (1 = Black in QR logic usually, but let's check polarity)
            // Standard QR: 1 is dark (module), 0 is light.
            int is_dark = (version3[byte_index] >> bit_pos) & 1;
            
            // 3. Convert to pixel color (0x00=Black, 0xFF=White)
            uint8_t color = is_dark ? 0x00 : 0xFF;

            // 4. Upscale 5x
            for (int dy = 0; dy < SCALE; dy++) {
                for (int dx = 0; dx < SCALE; dx++) {
                    dest_image[y * SCALE + dy][x * SCALE + dx] = color;
                }
            }
        }
    }
}

int main() {
    // 1. Perform the upscale
    unpack_and_upscale();

    // 2. Save to a PGM file (Portable Gray Map)
    // This is the simplest image format. You can open it in most viewers.
    FILE *fp = fopen("qrcode.pgm", "wb");
    if (!fp) {
        printf("Error: Could not open file for writing.\n");
        return 1;
    }

    // PGM Header: P5 (Binary), Width, Height, MaxVal
    fprintf(fp, "P5\n%d %d\n255\n", DEST_DIM, DEST_DIM);
    
    // Write pixel data
    fwrite(dest_image, sizeof(uint8_t), DEST_DIM * DEST_DIM, fp);
    
    fclose(fp);
    printf("Success! Saved 'qrcode.pgm' (%dx%d).\n", DEST_DIM, DEST_DIM);

    return 0;
}
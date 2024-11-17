#include "efi.h"

// Boot info structure
typedef struct {
    UINT64 FrameBufferBase;
    UINT32 FrameBufferWidth;
    UINT32 FrameBufferHeight;
    UINT32 FrameBufferPitch;
} BootInfo;

// A simple 8x8 bitmap font (example: only characters 'A', 'B', and space)
unsigned char font[3][8] = {
    { 0x18, 0x24, 0x24, 0x3C, 0x24, 0x24, 0x24, 0x00 }, // 'A'
    { 0x38, 0x24, 0x24, 0x38, 0x24, 0x24, 0x38, 0x00 }, // 'B'
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }  // ' ' (space)
};

// Function to draw a single character
void draw_char(UINT32 *framebuffer, UINT32 pitch, UINT32 x, UINT32 y, char c, UINT32 color) {
    if (c < 'A' || c > 'B') c = ' '; // Limit to 'A', 'B', or space
    int index = c - 'A';
    unsigned char *glyph = font[index];

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (glyph[row] & (1 << (7 - col))) { // Check if pixel is set
                framebuffer[(y + row) * pitch + (x + col)] = color;
            }
        }
    }
}

// Function to print a string
void print_text(UINT32 *framebuffer, UINT32 pitch, UINT32 x, UINT32 y, const char *text, UINT32 color) {
    while (*text) {
        draw_char(framebuffer, pitch, x, y, *text, color);
        x += 8; // Move to the next character position
        text++;
    }
}

__attribute__((noreturn)) void kernel_main(BootInfo *bootInfo) {
    UINT32 *framebuffer = (UINT32 *)bootInfo->FrameBufferBase;
    UINT32 pitch = bootInfo->FrameBufferPitch / 4; // Convert pitch from bytes to pixels

    // Clear the screen with black
    for (UINT32 y = 0; y < bootInfo->FrameBufferHeight; y++) {
        for (UINT32 x = 0; x < bootInfo->FrameBufferWidth; x++) {
            framebuffer[y * pitch + x] = 0x000000; // Black
        }
    }

    // Print "ABAB A" at (50, 50) with white color
    print_text(framebuffer, pitch, 50, 50, "ABAB A", 0xFFFFFF);

    while (1) __asm__("hlt");
}

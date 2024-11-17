__attribute__((noreturn)) void kernel_main(void) {
    char *video_memory = (char *)0xB8000; // VGA text mode memory
    video_memory[0] = 'K'; // Print 'K' at the top-left corner
    video_memory[1] = 0x07; // White text on black background
    while (1) __asm__("hlt");
}
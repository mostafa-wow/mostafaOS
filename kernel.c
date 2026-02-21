#include <stdint.h>

#define WIDTH 80
#define HEIGHT 25

uint16_t *vga_text_buffer = (uint16_t *)0xb8000;

uint8_t current_active_row = 0;
uint8_t current_active_column = 0;

void kputc(uint8_t c, uint8_t colour, uint8_t x, uint8_t y) {
  uint16_t pos = (y * 80) + x;
  vga_text_buffer[pos] = c | (colour << 8);
}

void kprint(uint8_t *s, uint8_t colour) {
  uint32_t i = 0;
  while (s[i] != 0) {
    if (s[i] == '\n') {
      current_active_row = 0;
      current_active_column++;
      i++;
      continue;
    }
    kputc(s[i], colour, current_active_row, current_active_column);
    i++;
    current_active_row++;
    if (current_active_row > WIDTH) {
      current_active_row = 0;
      current_active_column++;
    }
  }
}

void kernel_main() {
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  kprint("mostafaOS\n", 0x0F);
  return;
}

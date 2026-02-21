#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t *vga_text_buffer = (uint16_t *)0xb8000;

uint8_t current_active_row = 0;
uint8_t current_active_column = 0;

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void update_cursor(int x, int y) {
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void kputc(uint8_t c, uint8_t colour, uint8_t x, uint8_t y) {
  uint16_t pos = (y * VGA_WIDTH) + x;
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
    if (current_active_row > VGA_WIDTH) {
      current_active_row = 0;
      current_active_column++;
    }
  }
  update_cursor(current_active_row, current_active_column);
}

void kernel_main() {
  kprint("mostafaOS\n", 0x0F);
  kprint("This is my new self, Renewed. Recharged. Refocused. "
         "With nothing to stop me from being me.",
         0xF4);
  return;
}

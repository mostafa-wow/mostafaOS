#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t *vga_text_buffer = (uint16_t *)0xb8000;

uint8_t current_active_row = 0;
uint8_t current_active_column = 0;

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

char *itoa(int value, char *result, int base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  char *ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrst"
             "uvwxyz"[35 + (tmp_value - value * base)];
  } while (value);

  // Apply negative sign
  if (tmp_value < 0)
    *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}

void update_cursor(int x, int y) {
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void kputc(uint8_t c, uint8_t colour, uint8_t x, uint8_t y) {
  if (c == '\n') {
    current_active_column++;
    current_active_row = 0;
    return;
  }
  uint16_t pos = (y * VGA_WIDTH) + x;
  vga_text_buffer[pos] = c | (colour << 8);
}

void kprint(uint8_t *s, uint8_t colour) {
  uint32_t i = 0;
  while (s[i] != 0) {
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
         "With nothing to stop me from being me.\n",
         0xF4);

  kprint("value:", 0x0F);
  char num1[32];
  itoa(67, num1, 10);
  kprint(num1, 0x0F);

  kprint("\n", 0x0);

  kprint("value:", 0x0F);
  char num2[32];
  itoa(0x7c00, num2, 16);
  kprint(num2, 0x0F);

  return;
}

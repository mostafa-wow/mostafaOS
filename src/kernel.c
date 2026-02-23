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

void itoa(int32_t value, char *buffer) {
  char temp[12];
  int i = 0;
  int is_negative = 0;

  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return;
  }

  if (value < 0) {
    is_negative = 1;
    value = -value;
  }

  while (value > 0) {
    temp[i++] = '0' + (value % 10);
    value /= 10;
  }

  if (is_negative)
    temp[i++] = '-';

  // Reverse string into buffer
  int j;
  for (j = 0; j < i; j++)
    buffer[j] = temp[i - j - 1];

  buffer[i] = '\0';
}

void htoa(uint32_t value, char *buffer, int uppercase) {
  const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
  int i;
  for (i = 0; i < 8; i++) {
    uint8_t nibble = (value >> ((7 - i) * 4)) & 0xF;
    buffer[i] = digits[nibble];
  }
  buffer[8] = '\0';
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
  char number[32];
  itoa(676767, number);
  kprint("value:", 0x0F);
  kprint(number, 0x0F);
  return;
}

#include "include/types.h"

#define PORT 0x3f8

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

u16 *vga_text_buffer = (u16 *)0xb8000;

u8 current_active_row = 0;
u8 current_active_column = 0;

static inline void outb(u16 port, u8 val) {
  __asm__ volatile("outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline u8 inb(u16 port) {
  u8 ret;
  __asm__ volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

u8 *itoa(s32 value, u8 *result, s32 base) {
  // check that the base if valid
  if (base < 2 || base > 36) {
    *result = '\0';
    return result;
  }

  u8 *ptr = result, *ptr1 = result, tmp_char;
  s32 tmp_value;

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
  u16 pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (u8)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

void kputc(u8 c, u8 colour, u8 x, u8 y) {
  if (c == '\n') {
    current_active_column++;
    current_active_row = 0;
    return;
  }
  u16 pos = (y * VGA_WIDTH) + x;
  vga_text_buffer[pos] = c | (colour << 8);
}

void kprint(u8 *s, u8 colour) {
  u32 i = 0;
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

int8_t serial_init() {
  outb(PORT + 1, 0x00); // Disable all interrupts
  outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
  outb(PORT + 1, 0x00); //                  (hi byte)
  outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
  outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
  outb(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
  outb(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial
                        // returns same byte)

  // Check if serial is faulty (i.e: not same byte as sent)
  if (inb(PORT + 0) != 0xAE) {
    return 1;
  }

  // If serial is not faulty set it in normal operation mode
  // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
  outb(PORT + 4, 0x0F);
  return 0;
}

s32 serial_received() { return inb(PORT + 5) & 1; }

u8 read_serial() {
  while (serial_received() == 0)
    ;

  return inb(PORT);
}

s32 is_transmit_empty() { return inb(PORT + 5) & 0x20; }

void write_serial(u8 a) {
  while (is_transmit_empty() == 0)
    ;

  outb(PORT, a);
}

void serial_print(u8 *s) {
  s32 i = 0;
  while (s[i] != 0) {
    write_serial(s[i]);
    i++;
  }
}

void kernel_main() {
  kprint((u8 *)"mostafaOS\n", 0x0F);
  kprint((u8 *)"This is my new self, Renewed. Recharged. Refocused. "
               "With nothing to stop me from being me.\n",
         0xF4);

  kprint((u8 *)"value:", 0x0F);
  u8 num1[32];
  itoa(67, num1, 10);
  kprint(num1, 0x0F);

  kprint((u8 *)"\n", 0x0);

  kprint((u8 *)"value:", 0x0F);
  u8 num2[32];
  itoa(0x7c00, num2, 16);
  kprint(num2, 0x0F);

  serial_init();
  serial_print((u8 *)"mostafaOS\n");

  return;
}

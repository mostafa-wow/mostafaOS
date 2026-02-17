
void kernel_main() {
  *(char *)0xb8000 = 'M';
  return;
}

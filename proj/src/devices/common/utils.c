#include <lcom/lcf.h>

#include <stdint.h>


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) {printf("Pointer to store LSB points to NULL!\n"); return 1;}
  *lsb = val & 0x00FF;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) {printf("Pointer to store MSB points to NULL!\n"); return 1;}
  *msb = (val & 0xFF00) >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) {printf("Pointer to store value points to NULL!\n"); return 1;}
  
  uint32_t temp;
  if (sys_inb(port, &temp)) {printf("Error: sys_inb kernel call!\n"); return 1;}
  
  *value = temp;

  return 0;
}

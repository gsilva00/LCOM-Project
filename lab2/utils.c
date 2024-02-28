#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = (val >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  // internamente usa-se o sys_inb, nao da para usar direto porque retorna 32 bits, e so estamos usar 8 no value
  if (value == NULL) return 0;
  
  uint32_t temp = 0x00000000;
  sys_inb(port, &temp);
  
  *value = temp;
  
  return 1;
  }

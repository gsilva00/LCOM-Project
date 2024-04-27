#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t value, uint8_t *lsb) {
  if (lsb == NULL){
    return 1;
  }

  *lsb = value;
  return 0;
}

int(util_get_MSB)(uint16_t value, uint8_t *msb) {
  if (msb == NULL){
    return 1;
  }
  *msb = value >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL){
    return 1;
  }
  
  uint32_t temp = 0;
  if (sys_inb(port, &temp)){
    return 1;
  }
  
  *value = temp;

  return 0;
  }

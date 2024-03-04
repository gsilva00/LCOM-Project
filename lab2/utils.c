#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL) return 1;
  else{
    *lsb = val;
    return 0;
  }
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL) return 1;
  else{
    *msb = (uint16_t) val >> 8;
    return 0;
  }
}

int (util_sys_inb)(int port, uint8_t *value) {
  if(value == NULL) return 1;
  else{
    uint32_t temp = 0x00000000;
    if(sys_inb(port, &temp)) return 1;
    else{
      *value = temp;
      return 0;
    }
  }
}

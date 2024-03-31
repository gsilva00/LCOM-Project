#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = val >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;
  
  uint32_t temp;
  if (sys_inb(port, &temp)) return 1;
  
  *value = temp;
  
  return 0;
  }

/* NOTES: 
<About util_sys_inb()>
- Wrapper for the sys_inb() kernel call. 
- Reason: This kernel call takes the address of a 32-bit unsigned integer, instead of a 8-byte one. Therefore, it can't be called directly using the 'value' function argument.

*/

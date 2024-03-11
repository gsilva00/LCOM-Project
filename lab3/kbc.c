#include <lcom/lcf.h>
#include "i8254.h"

int read_kbc(int port, uint8_t *value){
  uint8_t status;
  if(util_sys_inb(0x64, &status)) return 1;
  if(status & BIT(0)) {
    if((status & BIT(7)) || (status & BIT(6))) return 1;
    if(util_sys_inb(0x60, value)) return 1;
  }
  else return 1;
  return 0;
}

int write_kbc(int port, uint32_t value){
  uint8_t status;
  if(util_sys_inb(0x64, &status)) return 1;
  if(!(status & BIT(1))) {
    if((status & BIT(7)) || (status & BIT(6))) return 1;
    if(sys_outb(0x60, value)) return 1;
  }else return 1;
  return 0;
}

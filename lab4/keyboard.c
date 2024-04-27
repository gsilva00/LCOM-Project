#include <lcom/lcf.h>

#include <stdint.h>

#include "keyboard.h"
#include "i8042keyboard.h"
#include "i8042mouse.h"


int verify_error_out_buf_by_status_mouse(uint8_t *output){
  int tries = 10;
  uint8_t status;

  while (tries--) {

    if (util_sys_inb(STAT_REG, &status)){
      return 1;
    }

    if (!(status & OBF)) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if ((status & PARITY_ERROR) || (status & TIMEOUT_ERROR) || !(status & AUX)){
      return 1;
    }

    if (util_sys_inb(OUT_BUF, output)){
      return 1;
    }

    return 0;
  }

  return 1;
}


int verify_error_out_buf_by_status(uint8_t *output) {
  int tries = 10;
  uint8_t status;

  while (tries) {
    tries--;
    if (util_sys_inb(STAT_REG, &status)){
      return 1;
    }

    if (!(status & OBF)) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if ((status & PARITY_ERROR) || (status & TIMEOUT_ERROR) || (status & AUX)){
      return 1;
    }

    if (util_sys_inb(OUT_BUF, output)){
      return 1;
    }

    return 0;
  }

  return 1;
}

int (util_sys_outb)(int port, uint8_t comando){
  int tries = 10;
  uint8_t status = 0;
  while (tries){
    tries--;

    if(util_sys_inb(STAT_REG,&status)){
      return 1;
    }
    if(status & IBF){
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }
    
    if(sys_outb(port, comando )){
      return 1;
    } 

    return 0;
  }

  return 1;
}

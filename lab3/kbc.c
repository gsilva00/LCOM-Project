#include "kbc.h"

int (kbc_read_output) (int port, uint8_t *output){
  uint8_t status = 0;
  int tries = 10;
  while(tries--){
    if(util_sys_inb(STATUS_REG, &status)){
      return 1;
    }
    if((status & OUT_BUF) != 0){
      if(util_sys_inb(port, output)){
        return 1;
      }
      if((status & PAR_ERR) != 0){
        return 1;
      }
      if((status & TM_ERR) != 0){
        return 1;
      }
      if((status & MS_DATA) != 0){
        return 1;
      }
      return 0;
    }else{
      tickdelay(micros_to_ticks(DELAY_US));
    }
  }
  return 1;
}

int read_kbc_cmd(int port, uint8_t *output){
  if(output == NULL){
    return 1;
  }
  if(util_sys_inb(port, output)){
    return 1;
  }
  return 0;
}

int write_kbc_cmd(int port, uint8_t input){
  if(sys_outb(port, input)){
    return 1;
  }
  return 0;
}

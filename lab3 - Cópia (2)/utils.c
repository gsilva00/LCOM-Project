#include "utils.h"

int (utils_sys_inb)(uint8_t port,uint8_t *value){
  if (value == NULL){
    return 1;
  }

  uint32_t temporario = 0;
  #define LAB3_a
  if(sys_inb(port,&temporario) ){
    return 1;
  }
  #ifdef LAB3_a
  count++;
  #endif
  *value = temporario;
  return 0;
}

int (utils_sys_outb)(uint8_t port,uint8_t comando){
  int tries = 10;
  uint8_t status = 0;
  printf("hm ");
  while (tries){
    printf(" %d",tries);
    if( utils_sys_inb(STATUS_REG, &status) ){
      printf("hm");
      return 1;
    }

    if(status & IBF){
      printf("not_yet...");
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }else{
      break;
    }
    tries--;
  }
  

  if(sys_outb(port,comando) ){
    printf("nao deu outb");
    return 1;
  }
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if(value == NULL){
    return 1;
  }
  uint32_t temporario;

  if( sys_inb(port,&temporario) ){
    return 1;
  }
  #ifdef LAB3
  count ++;
  #endif

  *value = temporario;  //temporario: 0x000000ff => value 0xff
  return 0;
}

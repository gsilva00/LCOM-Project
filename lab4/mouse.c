#include "mouse.h"
#include "keyboard.h"
#include <lcom/lcf.h>
#include "i8042.h"

int mouse_hook_id = 12;
uint8_t output = 0;

int mouse_subscribe_int(uint16_t *bit_no){
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(mouse_hook_id);
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

int mouse_unsubscribe_int(){
  return sys_irqrmpolicy(&mouse_hook_id);
}

void (mouse_ih)(){
  kbc_read_output(OUTBUF_REG, &output, 1);
}

int mouse_data_reporting(uint8_t cmd){
  int tries = 10;
  uint8_t response;
  while(tries--){
    if(write_kbc_cmd(INBUF_CMD_REG, BYTE_TO_MOUSE)){
      return 1;
    }
    if(write_kbc_cmd(INBUF_ARG_REG, cmd)){
      return 1;
    }
    tickdelay(micros_to_ticks(DELAY_US));
    if(kbc_read_output(OUTBUF_REG, &response, 1)){
      return 1;
    }
    if(response != ACK){
      return 1;
    }
  }
  return 0;
}

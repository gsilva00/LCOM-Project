#include <lcom/lcf.h>

#include <stdint.h>

#include "mouse.h"
#include "keyboard.h"
#include "i8042keyboard.h"
#include "i8042mouse.h"

extern uint8_t packetByte;
static int mouse_hookId = 12;

int(mouse_subscribe)(uint16_t *bit_no) {    //NOTA nornalmente é uint8_t * bit_no  MAS PRECISAMOS DE PELO MENOS UM BIT_NO QUE CONSIGA COLOCAR 1 NO BIT 12, então o mais pequeno é uint16_t
  if (bit_no == NULL) return 1;
  *bit_no = BIT(mouse_hookId);
  if( sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &mouse_hookId)){
    return 1;
  }
  return 0;
}

int(mouse_unsubscribe)() {
  if( sys_irqrmpolicy(&mouse_hookId)){
    return 1;
  }
  return 0;
}

void (mouse_ih)() {
  verify_error_out_buf_by_status_mouse(&packetByte);
}

int enable_stream_data(){
  int tries = 10;
  uint8_t foi_acknolegeado;

  while (tries) {
    tries--;
    if (util_sys_outb(COMAND_REG, MOUSE_WRITE)){
      return 1;
    }

    if (util_sys_outb(ARG_REG, DATA_ENABLE)){ 
      return 1;
    }

    tickdelay(micros_to_ticks(DELAY_US_MOUSE));

    //Para ler ACK byte e verificar se tá OK = 0xFA
    if (util_sys_inb(OUT_BUF, &foi_acknolegeado)){
      return 1;
    }
    if (foi_acknolegeado != ACK){
      return 1;
    }
  }
  return 0;
}

int disable_stream_data() {
  int tries = 10;
  uint8_t foi_acknolegeado;

  while (tries) {
    tries--;
    if (util_sys_outb(COMAND_REG, MOUSE_WRITE)){
      return 1;
    }

    if (util_sys_outb(ARG_REG, DATA_DIS)){
      return 1;
    }

    tickdelay(micros_to_ticks(DELAY_US_MOUSE));

    if (util_sys_inb(OUT_BUF, &foi_acknolegeado)){
      return 1;
    }

    if (foi_acknolegeado != ACK){
      return 1;
    }
  }

  return 0;
}

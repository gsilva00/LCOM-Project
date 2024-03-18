#include <lcom/lcf.h>
#include <stdint.h>
#include "i8042.h"
#include "mouse.h"

int mousehookid = 12;

int (mouse_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(mousehookid);
  if( sys_irqsetpolicy( KEYBOARD_IRQ ,IRQ_REENABLE | IRQ_EXCLUSIVE, &mousehookid ) ) {
    return 1;
  }
  return 0;
}

int (mouse_unsubscribe_int)() {
  if( sys_irqrmpolicy(&mousehookid) ){
    return 1;
  }
  return 0;
}

int (mouse_read_out_buf)( uint8_t *scancode){
  if(scancode == NULL){
    return 1;
  }
  if(util_sys_inb(OUT_BUF, scancode)){  //exceto o primeiro byte que é 0xE0
    return 1;
  }
  return 0;
}

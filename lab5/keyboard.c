#include "keyboard.h"

int kbd_hook_id = 1;
int tries = 10;
uint8_t output = 0;

int (kbd_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(kbd_hook_id);
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id);
}

int (kbd_unsubscribe_int)() {
  return sys_irqrmpolicy(&kbd_hook_id);
}

void (kbc_ih)() {
  if(kbc_read_output(OUTBUF_REG, &output, 0)){
    return;
  }
}

int (kbd_reset_int)(){
  uint8_t cmd_byte;
  if(write_kbc_cmd(INBUF_CMD_REG, READ_CMD_BYTE)){
    return 1;
  }
  if(read_kbc_cmd(OUTBUF_REG, &cmd_byte)){
    return 1;
  }
  cmd_byte |= BIT(0);
  if(write_kbc_cmd(INBUF_CMD_REG, WRITE_CMD_BYTE)){
    return 1;
  }
  if(write_kbc_cmd(INBUF_ARG_REG, cmd_byte)){
    return 1;
  }
  return 0;
}

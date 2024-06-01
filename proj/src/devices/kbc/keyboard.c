#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"


static int kbd_hookId = KBD_IRQ;
static uint8_t scancode;

int kbd_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = kbd_hookId;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbd_hookId);
}

int kbd_unsubscribe_int() {
  return sys_irqrmpolicy(&kbd_hookId);
}

void (kbc_ih)() {
  if (kbc_read_outbuf(OUTBUF_REG, &scancode, false)) {
    printf("Error while handling keyboard interrupt!\n");
  }
}

int reset_keyboard() {
  uint8_t cmd;

  if (kbc_write_cmd(CMD_REG, CMDBYTE_READ)) {
    printf("%s ERROR: kbc_write_cmd to KBC command register!\n", __func__);
    return 1;
  }
  if (kbc_read_outbuf(OUTBUF_REG, &cmd, false)) {
    printf("%s ERROR: kbc_read_outbuf in KBC Output Buffer!\n", __func__);
    return 1;
  }

  cmd |= CMDB_INT_KBD;
  if (kbc_write_cmd(CMD_REG, CMDBYTE_WRITE)) {
    printf("%s ERROR: kbc_write_cmd to KBC command register!\n", __func__);
    return 1;
  }
  if (kbc_write_cmd(INBUF_REG, cmd)) {
    printf("%s ERROR: kbc_write_cmd to KBC Input Buffer!\n", __func__);
    return 1;
  }

  return 0;
}


uint8_t get_scancode() {
  return scancode;
}

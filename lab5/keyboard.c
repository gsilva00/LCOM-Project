#include <lcom/lcf.h>

#include <stdint.h>

#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"

static int kbd_hookId = KBD_IRQ;
static uint8_t scancode;

int kbd_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbd_hookId;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbd_hookId);
}

int kbd_unsubscribe_int() {
  return sys_irqrmpolicy(&kbd_hookId);
}

void (kbc_ih)() {
  // Might need message to detect error
  kbc_read_outbuf(OUTBUF_REG, &scancode, false);
}

int reset_keyboard() {
  uint8_t cmd;

  if (kbc_write_cmd(CMD_REG, CMDBYTE_READ)) return 1;
  if (kbc_read_outbuf(OUTBUF_REG, &cmd, false)) return 1;

  cmd |= CMDB_INT_KBD;
  if (kbc_write_cmd(CMD_REG, CMDBYTE_WRITE)) return 1;
  if (kbc_write_cmd(INBUF_REG, cmd)) return 1;

  return 0;
}


uint8_t get_scancode() {
  return scancode;
}

#include <lcom/lcf.h>

#include <stdint.h>

#include "keyboard.h"
#include "kbc.h"
#include "i8042.h"

static int kbc_hookId = KBC_IRQ;
static uint8_t scancode;

int kbc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbc_hookId;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbc_hookId);
}

int kbc_unsubscribe_int() {
  return sys_irqrmpolicy(&kbc_hookId);
}

void (kbc_ih)() {
  // Might need message to detect error
  kbc_read_outbuf(KBC_OUTBUF, &scancode);
}

int reset_keyboard() {
  uint8_t cmd;

  if (kbc_write_cmd(KBC_CMD_REG, KBC_CMD_READ)) return 1;
  if (kbc_read_outbuf(KBC_OUTBUF, &cmd)) return 1;

  cmd |= CMDB_INT_KBD;
  if (kbc_write_cmd(KBC_CMD_REG, KBC_CMD_WRITE)) return 1;
  if (kbc_write_cmd(KBC_INBUF, cmd)) return 1;

  return 0;
}


uint8_t get_scancode() {
  return scancode;
}

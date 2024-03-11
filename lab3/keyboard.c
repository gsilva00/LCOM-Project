#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>

#include "i8042.h"

static int kbc_hookId;
static uint8_t scancode;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbc_hookId;
  return (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbc_hookId));
}

int(kbc_unsubscribe_int)() {
  return (sys_irqrmpolicy(&kbc_hookId));
}

void (kbc_ih)() {
  uint8_t kbc_st;

  if (util_sys_inb(KBC_ST_PORT, &kbc_st)) return 1;

  // When LSbit is 0 (no output) don't do anything
  if (!(kbc_st & KBC_ST_OUTBUF)) return 1;
  // Test for errors (2 MSbits on 1)
  if (kbc_st & KBC_ST_ERRPAR) return 1;
  if (kbc_st & KBC_ST_ERRTOUT) return 1;

  if (util_sys_inb(KBC_IO_PORT, &scancode)) return 1;

  tickdelay(DELAY_US);
  return 0;
}


uint8_t get_scancode() {
  return scancode;
}

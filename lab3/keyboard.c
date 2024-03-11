#include <lcom/lcf.h>
#include "keyboard.h"

#include <stdint.h>

#include "i8042.h"

static int kbc_hookId = KBC_IRQ;
static uint8_t scancode;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = kbc_hookId;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kbc_hookId);
}

int(kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&kbc_hookId);
}

void (kbc_ih)() {
  int tries = 5; // A more fault-tolerant approach is to give enough-time for the KBC or the keyboard to respond, retry a few times on time-out, and finally give up.
  while (tries) {
    uint8_t kbc_st;

    if (util_sys_inb(KBC_ST_PORT, &kbc_st)) return;

    // When LSbit is 0 (no output) don't do anything
    if (!(kbc_st & KBC_ST_OUTBUF)) return;
    // Test for errors (2 MSbits on 1)
    if (kbc_st & KBC_ST_ERRPAR) return;
    if (kbc_st & KBC_ST_ERRTOUT) return;

    if (util_sys_inb(KBC_IO_PORT, &scancode)) return;

    tries--;
    // tickdelay(micros_to_ticks(DELAY_US));
  }
}


uint8_t get_scancode() {
  return scancode;
}

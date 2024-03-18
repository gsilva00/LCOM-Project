#include <lcom/lcf.h>

#include <stdint.h>

#include "kbc.h"
#include "i8042.h"


int kbc_read_outbuf(uint8_t port, uint8_t *output, bool mouse) {
  int tries = MAX_TRIES;
  uint8_t kbc_st;

  while (tries--) {

    if (util_sys_inb(ST_REG, &kbc_st)) return 1;

    // When LSbit is 0 (outbuf empty) give KBC some time and try again
    if (!(kbc_st & ST_OBF)) {
      tickdelay(micros_to_ticks(KBD_DELAY_US));
      continue;
    }


    // else (outbuf is full), test for errors: 
    // Parity error in serial communication
    if (kbc_st & ST_ERRPAR) return 1;
    // Timeout error in serial communication
    if (kbc_st & ST_ERRTOUT) return 1;
    // AUX needs to be cleared (no mouse data) for keyboard reads
    if (kbc_st & ST_AUX && !mouse) return 1;
    // There needs to be mouse data for mouse reads
    if (!(kbc_st & ST_AUX) && mouse ) return 1;

    if (util_sys_inb(port, output)) return 1;
    return 0;
  }

  // Ran out of tries and didn't succeeed
  return 1;
}

int kbc_write_cmd(uint8_t port, uint8_t command) {
  int tries = MAX_TRIES;
  uint8_t kbc_st;

  while (tries--) {
    if (util_sys_inb(ST_REG, &kbc_st)) return 1;
    
    // When input buffer is full, give KBC some time and try again
    if (kbc_st & ST_IBF) {
      tickdelay(micros_to_ticks(KBD_DELAY_US));
      continue;
    }

    if (sys_outb(port, command)) return 1;
    return 0;
  }

  // Ran out of tries and didn't succeeed
  return 1;
}

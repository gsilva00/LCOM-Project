#include "kbc.h"
#include "i8042.h"


int kbc_read_outbuf(uint8_t port, uint8_t *output, bool mouse) {
  int tries = MAX_TRIES;
  uint8_t kbc_st;

  while (tries--) {

    if (util_sys_inb(ST_REG, &kbc_st)) {
      printf("ERROR: util_sys_inb from kbc status register!\n");
      return 1;
    }

    // When LSbit is 0 (outbuf empty) give KBC some time and try again
    if (!(kbc_st & ST_OBF)) {
      tickdelay(micros_to_ticks(KBD_DELAY_US));
      continue;
    }

    // Outbuf is full:
    // Read it before testing for errors. To discard it if there are errors.
    // If it was the other way around, the outbuf would stay full if the data was faulty
    if (util_sys_inb(port, output)) {
      printf("ERROR: util_sys_inb from port %x\n", port);
      return 1;
    }

    // Test for errors:
    if (kbc_st & ST_ERRPAR) {printf("KBC Parity Error!\n"); return 1;}
    if (kbc_st & ST_ERRTOUT) {printf("KBC Timeout Error!\n"); return 1;}
    if (kbc_st & ST_AUX && !mouse) {
      printf("KBC has mouse data during keyboard read! - ERROR!\n"); 
      return 1;
    }
    if (!(kbc_st & ST_AUX) && mouse) {
      printf("KBC doesn't have mouse data during mouse read! - ERROR!\n"); 
      return 1;
    }

    return 0;
  }

  printf("%s ERROR! Ran for %d tries and didn't succeed!\n", __func__, tries);
  return 1;
}

int kbc_write_cmd(uint8_t port, uint8_t command) {
  int tries = MAX_TRIES;
  uint8_t kbc_st;

  while (tries--) {
    if (util_sys_inb(ST_REG, &kbc_st)) {
      printf("%s ERROR!\n", __func__);
      return 1;
    }
    
    // When input buffer is full, give KBC some time and try again
    if (kbc_st & ST_IBF) {
      tickdelay(micros_to_ticks(KBD_DELAY_US));
      continue;
    }

    if (sys_outb(port, command)) {printf("ERROR: sys_outb to port %x!\n", port); return 1;}
    return 0;
  }

  printf("%s ERROR! Ran for %d tries and didn't succeed!\n", __func__, tries);
  return 1;
}

int kbc_write_mouse(uint8_t arg) {
  int tries = MAX_TRIES;
  uint8_t ack_message;

  while (tries--) {
    if (kbc_write_cmd(CMD_REG, MOUSE_WRITE)) {
      printf("%s ERROR: kbc_write_cmd to KBC command register!\n", __func__);
      return 1;
    }

    if (kbc_write_cmd(INBUF_REG, arg)) {
      printf("%s ERROR: kbc_write_cmd to KBC command's arguments register!\n", __func__);
      return 1;
    }
    
    tickdelay(micros_to_ticks(MOUSE_DELAY_US));
    if (util_sys_inb(OUTBUF_REG, &ack_message)) {
      printf("%s ERROR: util_sys_inb while reading ACK!\n", __func__);
      return 1;
    }
    if (ack_message == PS2_ACK) return 0;
  }

  printf("%s ERROR! Ran for %d tries and didn't succeed!\n", __func__, tries);
  return 1;
}

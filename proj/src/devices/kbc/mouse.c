#include "mouse.h"
#include "kbc.h"
#include "i8042.h"


static int mouse_hookId = MOUSE_IRQ;
static uint8_t packetByte;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = mouse_hookId;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &mouse_hookId);
}

int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&mouse_hookId);
}

void (mouse_ih)() {
  if (kbc_read_outbuf(OUTBUF_REG, &packetByte, 1)) {
    printf("Error while handling mouse interrupt!\n");
  }
}


int enable_stream_data() {
  return kbc_write_mouse(PS2_DATA_ENABLE);
}

int disable_stream_data() {
  return kbc_write_mouse(PS2_DATA_DIS);
}


uint8_t get_packetByte() {
  return packetByte;
}

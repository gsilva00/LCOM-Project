#include <lcom/lcf.h>

#include <stdint.h>

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
  kbc_read_outbuf(OUTBUF_REG, &packetByte, 1);
}


int enable_stream_data() {
  return kbc_write_mouse(PS2_DATA_ENABLE);
}

// Read notes below
int disable_stream_data() {
  return kbc_write_mouse(PS2_DATA_DIS);
}


uint8_t get_packetByte() {
  return packetByte;
}


/* NOTES: 
<About disable_stream_data()>
 - This function is sending the PS2_DATA_DIS command to the mouse. Might separate concerns into another function if need to send another command to the mouse
 - From https://web.archive.org/web/20210708233930/https://www.aquaphoenix.com/hardware/ledlamp/reference/synaptics_touchpad_interfacing_guide.pdf (page 31): 
  - "The device responds within 25ms (25000 μs), unless the host prevents it from doing so by inhibiting the bus."
  - "Each command or argument byte produces at least one response byte from the device.", 
  - "When the host gets an $FE response, it should retry the offending command." - so it continues, to not run the argument part;
  - "If an argument byte elicits an $FE response, the host should retransmit the entire command, not just the argument byte." - then why doesn't it work when I test the ack after both the command and the arguments?
 - This means that both the command and the argument can send an $FE response, so we need to check for both

*/

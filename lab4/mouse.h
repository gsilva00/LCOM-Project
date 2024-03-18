#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>

extern uint8_t bit_no;
extern uint32_t count;
extern uint8_t bit_no_timer;
extern uint8_t counter;

int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();
int (keyboard_read_out_buf)(uint8_t *scancode);


#endif /* __KEYBOARD_H */

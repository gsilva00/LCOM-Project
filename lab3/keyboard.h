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
int (verify_error_out_buf_by_status)();
void (kbc_ih)();
int (verify_error_out_buf_by_status_and_see_availabity)();
int (enable_interrupt)();
void (kbc_timed_ih)(uint8_t n);

#endif /* __KEYBOARD_H */

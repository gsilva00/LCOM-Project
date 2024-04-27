
#include <stdbool.h>
#include <lcom/lcf.h>
#include "i8042.h"

int (kbd_subscribe)(uint8_t * bit_no);
int (kbd_unsubscribe)();
int (verify_error)();
int (enable_interrupt)();
int (keyboard_read_out_buf)( uint8_t *scancode);
int (verify_error_out_buf_by_status_and_see_availabity)();


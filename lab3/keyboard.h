#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "i8254.h"

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();

#endif

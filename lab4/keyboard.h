#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "kbc.h"

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
int (kbd_reset_int)();

#endif

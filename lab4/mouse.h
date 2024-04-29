#ifndef __MOUSE_H
#define __MOUSE_H
#include <lcom/lcf.h>
#include "i8042.h"

int mouse_subscribe_int(uint16_t *bit_no);
int mouse_unsubscribe_int();
int mouse_data_reporting(uint8_t cmd);

#endif

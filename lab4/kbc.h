#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

int (kbc_read_output)(int port, uint8_t *output, bool mouse);
int read_kbc_cmd(int port, uint8_t *output);
int write_kbc_cmd(int port, uint8_t input);

#endif

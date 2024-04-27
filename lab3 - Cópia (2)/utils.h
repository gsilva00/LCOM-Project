#include <lcom/lcf.h>

#include <stdbool.h>

#include "i8042.h"

extern uint32_t count;

int (utils_sys_inb)(uint8_t port,uint8_t *value);
int (utils_sys_outb)(uint8_t port,uint8_t comando);
int (util_sys_inb)(int port, uint8_t *value);

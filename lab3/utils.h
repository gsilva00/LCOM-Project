#include <lcom/lcf.h>

#include "ikbd.h"
#include <stdint.h>

int(utils_get_LSB)(uint16_t val, uint8_t *lsb);
int(utils_get_MSB)(uint16_t val, uint8_t *msb);
int (utils_sys_inb)(int port, uint8_t *value) ;
int (util_sys_outb)(int port, uint8_t value) ;
int (verify_error_status)(int port);

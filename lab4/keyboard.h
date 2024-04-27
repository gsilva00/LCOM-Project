#ifndef KBC_H
#define KBC_H

#include <stdint.h>


int (verify_error_out_buf_by_status_mouse)( uint8_t *output);
int (verify_error_out_buf_by_status)( uint8_t *output);
int (util_sys_outb)(int port, uint8_t comando);

int kbc_read_outbuf(uint8_t port, uint8_t *output, bool mouse);


int kbc_write_cmd(uint8_t port, uint8_t command);

#endif

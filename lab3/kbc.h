#ifndef KBC_H
#define KBC_H

#include <stdint.h>

/**
 * @brief Bottom layer function: Reads KBC output buffer
 * 
 * @param port address to a KBC output register
 * @param output address of memory to receive the byte from the port
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_read_outbuf(uint8_t port, uint8_t *output);

/**
 * @brief Bottom layer function: Writes a command to an input buffer
 * 
 * @param port address to a KBC output register
 * @param command KBC's Command Byte to be written to a buffer
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_write_cmd(uint8_t port, uint8_t command);

#endif

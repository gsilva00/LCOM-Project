#ifndef KBC_H
#define KBC_H

#include <stdint.h>


/**
 * @brief Bottom layer function: Reads from an output register
 * 
 * @param port address to a KBC output register
 * @param output address of memory to receive the byte from the port
 * @param mouse True to read output from mouse, false to read output from keyboard
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_read_outbuf(uint8_t port, uint8_t *output, bool mouse);

/**
 * @brief Bottom layer function: Writes a command to an input register
 * 
 * @param port address to a KBC input register
 * @param command KBC's Command Byte to be written to a register
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_write_cmd(uint8_t port, uint8_t command);

/**
 * @brief Bottom layer function: Writes a command to an input register
 * 
 * @param mouseArgument 0xD4-Command's Argument Byte to be written directly to mouse
 * @return Return 0 upon success and non-zero otherwise
 */
int kbc_write_mouse(uint8_t mouseArgument);

#endif

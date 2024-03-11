#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdint.h>


/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_unsubscribe_int)();

uint8_t get_scancode();
uint8_t get_sysinb_count();

#endif /* __KEYBOARD_H */

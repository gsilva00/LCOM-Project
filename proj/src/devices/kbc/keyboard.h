#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <lcom/lcf.h>

#include <stdint.h>


/**
 * @brief Subscribes and enables keyboard interrupts
 * @details Uses the IRQ_EXCLUSIVE flag to avoid conflicts with Minix's default keyboard IH
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success and non-zero otherwise
 */
int kbd_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes keyboard interrupts
 *
 * @return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Re-enables keyboard interrupts (that lcf_start() disabled) by writing the appropriate command byte
 *
 * @return 0 upon success and non-zero otherwise
 */
int reset_keyboard();


/**
 * @brief Getter for the scancode static global variable
 * @details Getter is needed because of the use of the static keyword (makes the variable only accesible in the file it's declared). Provides encapsulation
 * 
 * @return uint8_t value corresponding to the scancode
 */
uint8_t get_scancode();


#endif /* KEYBOARD_H */

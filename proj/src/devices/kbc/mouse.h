#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>

#include <stdint.h>


/**
 * @brief Subscribes and enables mouse interrupts
 * @details Uses the IRQ_EXCLUSIVE flag to avoid conflicts with Minix's default mouse IH
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts
 *
 * @return 0 upon success and non-zero otherwise
 */
int(mouse_unsubscribe_int)();


/**
 * @brief Enable stream mode data reporting
 *
 * @return 0 upon success and non-zero otherwise
 */
int enable_stream_data();

/**
 * @brief Disable stream mode data reporting
 *
 * @return 0 upon success and non-zero otherwise
 */
int disable_stream_data();



/**
 * @brief Getter for the packetByte static global variable
 * @details Getter is needed because of the use of the static keyword (makes the variable only accesible in the file it's declared). Provides encapsulation
 * 
 * @return uint8_t value corresponding to the packet's byte
 */
uint8_t get_packetByte();


#endif /* MOUSE_H */

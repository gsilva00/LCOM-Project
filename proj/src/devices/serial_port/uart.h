#ifndef UART_H
#define UART_H

#include <lcom/lcf.h>

#include <stdint.h>

#include "queue.h"


/**
 * @brief Configures the UART's initial state
 * @details Enables interrupts for when data is available and for when it's possible to send data. Stores the default configuration for resetting at the end of execution. Otherwise maintains the default configuration (by reading it first) except for the interrupts activated. Initializes the queue static global variable for receiving data
 *
 * @return 0 upon success, non-zero otherwise
 */
int configure_uart();

/**
 * @brief Resets UART to its default state
 * @details Sends the previously stored MINIX UART configuration (during configure_uart()) to the appropriate register. Deallocates the queue.
 * 
 * @return 0 upon success, non-zero otherwise
*/
int reset_uart();

/**
 * @brief Subscribes and enables UART interrupts
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success and non-zero otherwise
 */
int uart_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes RTC interrupts
 *
 * @return 0 upon success and non-zero otherwise
 */
int uart_unsubscribe_int();

/**
 * @brief RTC's Interrupt Handler
 * @details Only 1 type of interrupt is being handled (Received Data Available Interrupt), because the others aren't activated configure_uart(). While there is data in the Receiver Buffer Register, receive_char() is called. 
*/
void uart_ih();

/**
 * @brief Receives a character from the UART
 * @details Reads a character from the Receiver Buffer Register. If there are no errors, it pushes the character into the queue
 *
 * @param st int8_t variable holding the status
 * @return 0 upon success, non-zero otherwise
 */
int receive_char(uint8_t st);

/**
 * @brief Sends a character to the UART
 * @details Outputs a character to the Transmitter Holding Register.
 *
 * @param chr int8_t variable holding the character to send
 * @return 0 upon success, non-zero otherwise
 */
int send_char(uint8_t chr);

/**
 * @brief Getter for the receiving queue (stores the data read from the UART)
 * @details Getter is needed because of the use of the static keyword (makes the variable only accesible in the file it's declared). Provides encapsulation
 * 
 * @return Pointer to the queue_t struct corresponding to the receiving queue
 */
queue_t* get_receive_q();


#endif

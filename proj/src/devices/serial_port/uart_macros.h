#ifndef UART_MACROS_H
#define UART_MACROS_H

#include <lcom/lcf.h>


/**
 * @defgroup UARTMacros UART Macros
 * @brief Constants for programming the UART - Universal Asynchronous Receiver-Transmitter (Serial-Port).
 * @{
 */


/** 
 * @defgroup NumConsts8042 KBC Numeric Constants
 * @ingroup i8042
 * @brief Numeric Constants (Constants and bytes for synchronization)
 * @{
 */
#define UART_SYNC_MAX_FRAMES 300
#define UART_SYNC 0x00
#define UART_ACK 0x01
/** @} */


/** 
 * @defgroup UARTIRQ UART's IRQs
 * @ingroup UARTMacros
 * @brief UART Interrupt Request Lines: 
 * @{
 */
#define COM1_IRQ 4 /**< @brief COM1's IRQ line */
#define COM2_IRQ 3 /**< @brief COM2's IRQ line - Not used */
/** @} */

/**
 * @defgroup UARTPorts UART's Ports
 * @ingroup UARTMacros
 * @brief Each UART takes:
 * - 8 consecutive port numbers in the PC's I/O address space
 * - 1 IRQ line 
 * @{
 */
#define COM1_PORT 0x3F8 /**< @brief COM1's base address */
#define COM2_PORT 0x2F8 /**< @brief COM2's base address - Not used */
/** @} */

/**
 * @defgroup UARTRegs UART's Registers 
 * @ingroup UARTMacros
 * @brief 8-bit registers' addresses
 * @{
 */
#define UART_SR 7   /**< @brief Scratchpad Register (Read / Write) */
#define UART_MSR 6  /**< @brief Modem Status Register (R) */
#define UART_LSR 5  /**< @brief Line Status Register (R) */
#define UART_MCR 4  /**< @brief Modem Control Register (R/W) */
#define UART_LCR 3  /**< @brief Line Control Register (R/W) */
#define UART_FCR 2  /**< @brief FIFO Control Register (W) */
#define UART_IIR 2  /**< @brief Interrupt Identification Reg (R) */
#define UART_DLM 1  /**< @brief Divisor Latch MSB (Overloaded address - accessible if DLAB of LCR is 1) */
#define UART_IER 1  /**< @brief Interrupt Enable Register (R/W) */
#define UART_DLL 0  /**< @brief Divisor Latch LSB (Overloaded address - accessible if DLAB of LCR is 1) */
#define UART_THR 0  /**< @brief Transmitter Holding Register - Sending character (W) */
#define UART_RBR 0  /**< @brief RBR Receiver Buffer Register - Read received character (R) */
/** @} */


/** 
 * @defgroup StatusReg Line Status Register (LST)
 * @ingroup UARTMacros
 * @brief Bits 0 to 4 reset on LSR read
 * @{
*/
#define LSR_ERRFIFO BIT(7)  /**< @brief ERRPAR/ERRFRAME/BREAK in the FIFO; Resets to 0 on LSR read if no errors */
#define LSR_TMT_MT BIT(6)   /**< @brief Both THR and Transmitter Shift Register Empty */
#define LSR_THR_RDY BIT(5)  /**< @brief Ready to accept new character for transmitting */
#define LSR_BREAKINT BIT(4) /**< @brief Serial data input line at low for longer than full-word transmission */
#define LSR_ERRFRAME BIT(3) /**< @brief Received character with no valid stop bit */
#define LSR_ERRPAR BIT(2)   /**< @brief Received character has parity error */
#define LSR_ERROVER BIT(1)  /**< @brief Received character overwritten by another one */
#define LSR_DATA BIT(0)     /**< @brief There is data for receiving */
/** @} */


/** Line Control Register (LCR): Set main communication parameters 
 * - DLAB (1 for Divisor Latch Access - DLL and DLM; 0 for RBR or THR)
 * - Break Control (sets serial output to 0) - Bit 6
 * - Parity (No-, odd-, even-parity; parity bit always 1, always 0) - Bits 5, 4, 3
 * - Number of stop bits (0 for 1 stop bit; 1 for 2 stop bits, 1.5 when 5 bits per char) - Bit 2
 * - Number of bits per character (5, 6, 7 8) - Bits 1 and 0
*/

/**
 * @defgroup IntEnableReg Interrupt Enable Register (IER)
 * @ingroup UARTMacros
 * @brief Controlls UART interrupts on some events
 * @{
*/
// Modem status interrupt omitted
#define LSRERR_INT BIT(2)  /**< @brief Enables the Receiver Line Status Interrupt This event is generated when there is a change in the state of bits 1 to 4, i.e. the error bits, of the LSR */
#define THR_RDY_INT BIT(1)  /**< @brief Enables the Transmitter Holding Register Empty Interrupt */
#define DATA_RDY_INT BIT(0) /**< @brief Enables the Received Data Available Interrupt */
/** @} */


/**
 * @defgroup IntIdReg Interrupt Identification Register (IIR)
 * @ingroup UARTMacros
 * @details This register:
 * - Records source of interrupts;
 * - Priority of interrupts: LSRERR then DATARDY then THRRDY then Modem Status;
 * - When read, UART freezes all interrupts and indicates the highest priority pending one;
 * When there's an interrupt, bits 3,2,1 are the important ones. The interrupt sources are ordered by priority.
 * @{
*/
#define IIR_LSRERR BIT(2) | BIT(1)    /**< @brief Receiver Line Status */
#define IIR_DATA_RDY BIT(2)            /**< @brief Received Data Available */
#define IIR_CHAR_TOUT BIT(3) | BIT(2) /**< @brief Character Timeout (FIFO) */
#define IIR_THR_RDY BIT(1)             /**< @brief Transmitter Holding Register Empty */
// Modem status interrupt omitted
#define IIR_NONE BIT(0)               /**< @brief When set, no interrupt is pending */
/** @} */

/** @} */


#endif // UART_MACROS_H

#ifndef LCOM_I8042_H
#define LCOM_I8042_H

#include <lcom/lcf.h>


/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC - Keyboard (and Mouse) Controller.
 */

// Numeric Constants:
#define MAX_TRIES 10 /** @brief Maximum amount of tries allowed to successfully complete operation on KBC */
#define KBD_DELAY_US 20000 /** @brief Keyboard's time to respond to a command in microseconds (μs) */
#define MOUSE_DELAY_US 25000 /** @brief Mouse's time to send an acknowledgement message in microseconds (μs) */

// KBC Interrupt Request Lines:
#define KBD_IRQ 1 /** @brief Keyboard's IRQ line */
#define MOUSE_IRQ 12 /** @brief Mouse's IRQ line */

// KBC Registers and Ports:
#define INBUF_REG 0x60 /** @brief Input Register to write KBD commands or KBC commands' arguments */
#define OUTBUF_REG 0x60 /** @brief Output Register to read scancodes from KBD or return values of KBC commands/KBC commands' arguments */
#define ST_REG 0x64 /** @brief Status Register to read KBC status */
#define CMD_REG 0x64 /** @brief Not Named Register to write KBC commands */ 

// KBC Status Byte:
#define ST_ERRPAR BIT(7) /** @brief Status byte's parity error bit - invalid data (error in serial communication) */
#define ST_ERRTOUT BIT(6) /** @brief Status byte's timeout error bit - invalid data (error in serial communication) */
#define ST_AUX BIT(5) /** @brief Status byte's mouse data bit */
#define ST_IBF BIT(1) /** @brief Status byte's input buffer full bit */
#define ST_OBF BIT(0) /** @brief Status byte's output buffer full bit */

// Auxiliars for Scancodes:
#define BREAKCODE BIT(7) /** @brief Makecode MSbit is 0, Breakcode MSbit is 1 */
#define SC_MSB1 0xE0 /** @brief 2-byte-scancode MSB can be E0 */
#define SC_MSB2 0xE2 /** @brief 2-byte-scancode MSB can also be E2 */
#define BREAKCODE_ESC 0x81 /** @brief Breakcode of ESC key */
#define MAKECODE_P 0x19 /** @brief Makecode of P key */
#define MAKECODE_A 0x1e /** @brief Makecode of A key */
#define MAKECODE_RIGHT 0x4d /** @brief Makecode of right key */
#define MAKECODE_LEFT 0x4b /** @brief Makecode of left key */
#define BREAKCODE_LEFT 0xcd /** @brief Breakcode of left key */
#define BREAKCODE_RIGHT 0xcb /** @brief Breakcode of right key */
#define MAKECODE_UP 0x48 /** @brief Makecode of up key */
#define MAKECODE_DOWN 0x50 /** @brief Makecode of down key */
#define MAKECODE_ENTER 0x1C /** @brief Makecode of enter key */

// KBC Commands (written to CMD_REG):
#define CMDBYTE_READ 0x20 /** @brief To be able to read Command Byte from KBC_OUTBUF */
#define CMDBYTE_WRITE 0x60 /** @brief To be able to write new Command Byte to KBC_INBUF */
#define MOUSE_WRITE 0xD4 /** @brief To be able to write byte arguments directly to mouse without KBC interpretation (through KBC_INBUF) */

// KBC Command Byte (returned on OUTBUF_REG after CMDBYTE_READ):
#define CMDB_DIS_MOUSE BIT(5) /** @brief Bit to disable mouse interface */
#define CMDB_DIS_KBD BIT(4) /** @brief Bit to disable keyboard interface */
#define CMDB_INT_MOUSE BIT(1) /** @brief Bit to enable interrupt on OBF, from mouse */
#define CMDB_INT_KBD BIT(0) /** @brief Bit to enable interrupt on OBF, from keyboard; */


// PS/2 Mouse Data Packet 1's byte:
#define PS2_YOVER BIT(7) /** @brief Flag - mouse Y displacement too large to represent as a 9-bit 2's complement integer */
#define PS2_XOVER BIT(6) /** @brief Flag - mouse X displacement too large to represent as a 9-bit 2's complement integer */
#define PS2_YDELTA BIT(5) /** @brief Y axis relative displacement MSBit of 9-bit 2's complement integer - indicates if Y delta is negative */
#define PS2_XDELTA BIT(4) /** @brief X axis relative displacement MSBit of 9-bit 2's complement integer - indicates if X delta is negative */
#define PS2_FIRSTID BIT(3) /** @brief Bit 3 of 1st byte is always set */
#define PS2_MB BIT(2) /** @brief State of the middle button */
#define PS2_RB BIT(1) /** @brief State of the right button */
#define PS2_LB BIT(0) /** @brief State of the left button */


// PS/2 Acknowledgement Bytes:
#define PS2_ACK 0xFA /** @brief If everything OK */
#define PS2_NACK 0xFE /** @brief If invalid byte (may be because of a serial communication error) */
#define PS2_ERROR 0xFC /** @brief Second consecutive invalid byte */

// MOUSE_WRITE's arguments (passed in KBC_INBUF, responses to them in KBC_OUTBUF) - Directly to the mouse:
// #define PS2_RESET 0xFF /** @brief Mouse reset */
// #define PS2_RESEND 0xFE /** @brief For serial communications errors */
// #define PS2_DEFAULT 0xF6 /** @brief Set default values */
#define PS2_DATA_DIS 0xF5 /** @brief Disable Data Reporting - In stream mode, should be sent before any other command */
#define PS2_DATA_ENABLE 0xF4 /** @brief Enable Data Reporting - In stream mody only */
// #define PS2_SAMPLE 0xF3 /** @brief Sets state sampling rate */
// #define PS2_REMOTE 0xF0 /** @brief Set remote mode - sends data on request only */
// #define PS2_READ_DATA 0xEB /** @brief Send data packet request  */
// #define PS2_STREAM 0xEA /** @brief Set Stream Mode */
// #define PS2_ST_REQ 0xE9 /** @brief Status request - get mouse configuration (3 bytes) */
// #define PS2_SET_RES 0xE8 /** @brief  */
// #define PS2_SCALING2 0xE7 /** @brief Scaling parameter: value of counters reported by mouse - Acceleration mode */
// #define PS2_SCALING1 0xE6 /** @brief Scaling parameter: value of counters reported by mouse - Linear mode */

/**@}*/

#endif /* LCOM_I8042_H */

#ifndef LCOM_I8042_H
#define LCOM_I8042_H

#include <lcom/lcf.h>


#define MAX_TRIES 10 /** @brief Maximum amount of tries allowed to successfully complete operation on KBC */
#define KBD_DELAY_US 20000 /** @brief Keyboard's time to respond to a command in microseconds (μs)*/

#define KBC_IRQ 1 /** @brief KBC (Keyboard Controller) IRQ line */

#define KBC_INBUF 0x60 /** @brief Input Register (to write KBD commands or KBC commands' arguments) */
#define KBC_OUTBUF 0x60 /** @brief Output Register (to read scancodes from KBD or KBC commands' return values) */
#define KBC_ST_REG 0x64 /** @brief Status Register (to read KBC status) */
#define KBC_CMD_REG 0x64 /** @brief Not Named Register (to write KBC commands) */ 

#define KBC_ST_ERRPAR BIT(7) /** @brief Status byte's parity error */
#define KBC_ST_ERRTOUT BIT(6) /** @brief Status byte's timeout error */
#define KBC_ST_AUX BIT(5) /** @brief Status byte's mouse data */
#define KBC_ST_INBUF BIT(1) /** @brief Status byte's input buffer full */
#define KBC_ST_OUTBUF BIT(0) /** @brief Status byte's output buffer full */

#define BREAKCODE BIT(7) /** @brief Makecode MSbit is 0, Breakcode MSbit is 1 */
#define SC_MSB1 0xE0 /** @brief 2-byte-scancode MSB can be E0 */
#define SC_MSB2 0xE2 /** @brief 2-byte-scancode MSB can also be E2 */
#define BREAKCODE_ESC 0x81 /** @brief Breakcode of ESC key */

#define KBC_CMD_READ 0x20 /** @brief KBC Command (written to KBC_ST_PORT). To be able to read Command Byte from KBC_IO_PORT */
#define KBC_CMD_WRITE 0x60 /** @brief KBC Command (written to KBC_ST_PORT). To be able to write new Command Byte to KBC_IO_PORT */

#define CMDB_DIS_MOUSE BIT(5) /** @brief KBC Command Byte - Bit to disable mouse interface */
#define CMDB_DIS_KBD BIT(4) /** @brief KBC Command Byte - Bit to disable keyboard interface */
#define CMDB_INT_MOUSE BIT(1) /** @brief KBC Command Byte - Bit to enable interrupt on OBF, from mouse */
#define CMDB_INT_KBD BIT(0) /** @brief KBC Command Byte - Bit to enable interrupt on OBF, from keyboard; */


#endif /* LCOM_I8042_H */

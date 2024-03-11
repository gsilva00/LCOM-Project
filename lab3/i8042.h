#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define DELAY_US 20000 /** @brief Keyboard's time to respond to a command in microseconds (μs)*/

#define KBC_IRQ 1 /** @brief KBC (Keyboard Controller) IRQ line */

#define KBC_IO_PORT 0x60 /** @brief KBC (Keyboard Controller) Input and Output port */
#define KBC_ST_PORT 0x64 /** @brief KBC (Keyboard Controller) Status port */

#define KBC_ST_ERRPAR BIT(7) /** @brief Status byte parity error */
#define KBC_ST_ERRTOUT BIT(6) /** @brief Status byte timeout error */
#define KBC_ST_INBUF BIT(1) /** @brief Status byte input buffer full */
#define KBC_ST_OUTBUF BIT(0) /** @brief Status byte output buffer full */

#define MAKECODE BIT(7) /** @brief Makecode MSbit is 0, Breakcode MSbit is 1 */
#define SC_MSB1 0xE0 /** @brief 2-byte-scancode MSB can be E0 */
#define SC_MSB2 0xE2 /** @brief 2-byte-scancode MSB can also be E2 */

#endif /* _LCOM_I8054_H */

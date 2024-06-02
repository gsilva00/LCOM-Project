#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

/** 
 * @defgroup i8254 i8254 Macros
 * @brief Constants for programming the i8254 Timer
 * @{
 */

/** 
 * @defgroup NumConsts8254 Timer's Numeric Constants
 * @ingroup i8254
 * @brief Numeric Constants (frequencies and other miscellaneous stuff)
 * @{
 */
#define TIMER_MAX_FREQ 1193182  /**< @brief maximum frequency for timer in PC and AT */
#define TIMER0_IRQ 0            /**< @brief Timer 0 IRQ line */
/** @} */

/**
 * @defgroup TimerPorts Timer's I/O Ports
 * @ingroup i8254
 * @brief Timer's ports' addresses
 * @{
 */
#define TIMER_0 0x40      /**< @brief Timer 0 count register */
#define TIMER_1 0x41      /**< @brief Timer 1 count register */
#define TIMER_2 0x42      /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43   /**< @brief Control register */
#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */
/** @} */

/** 
 * @defgroup TimerSel Timer selection for Control Word
 * @ingroup i8254
 * @brief Counter selection: bits 7 and 6
 * @{
 */
#define TIMER_SEL0 0x00                /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1 BIT(6)              /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2 BIT(7)              /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6)) /**< @brief Read-Back Command */
/** @} */

/** 
 * @defgroup RegSel Register selection for Control Word
 * @ingroup i8254
 * @brief Register selection: bits 5 and 4 of Control Word
 * @details In what way the counters of the Timer will be initialized
 * @{
 */
#define TIMER_LSB BIT(4)                      /**< @brief Initialize Counter LSB only */
#define TIMER_MSB BIT(5)                      /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) /**< @brief Initialize LSB first and MSB afterwards */
/** @} */

/** 
 * @defgroup OpMode Operating mode for Control Word
 * @ingroup i8254
 * @brief Operating mode: bits 3, 2 and 1 of Control Word
 * @{
 */
#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) /**< @brief Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            /**< @brief Mode 2: rate generator */
/** @} */

/** 
 * @defgroup CountMode Counting Mode for Control Word
 * @ingroup i8254
 * @brief Counting mode: bit 0 of Control Word
 * @{
 */
#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */
/** @} */

/** 
 * @defgroup RBCommand Read-Back Command Byte
 * @ingroup i8254
 * @brief Meaning of each of the bits of the Read-Back Command Byte
 * @{ 
 */
#define TIMER_RB_COUNT_ BIT(5) /**< @brief Active low @details When set to 1, the output of the Read-Back Command won't have the count info (count disabled) */
#define TIMER_RB_STATUS_ BIT(4) /**< @brief Active low @details When set to 1, the output of the Read-Back Command won't have the status info (status disabled) */
#define TIMER_RB_SEL(n) BIT((n) + 1) /**< @brief Select which counter the info will pertain to */
/** @} */

/** @} */

#endif /* _LCOM_I8254_H */

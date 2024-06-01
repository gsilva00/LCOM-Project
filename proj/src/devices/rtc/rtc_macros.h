#ifndef RTC_MACROS_H
#define RTC_MACROS_H

#include <lcom/lcf.h>


/** 
 * @defgroup RTCMacros RTC Macros
 * @brief Constants for programming the RTC - Real-Time Clock.
 * @{
 */

#define RTC_IRQ 8 /**< @brief RTC's IRQ Line */

/** 
 * @defgroup NumConstsRTC RTC Numeric Constants
 * @ingroup RTCMacros
 * @brief Numeric Constants
 * @{
 */
#define MAX_READS 10
/** @} */

/** 
 * @defgroup RTCMain RTC Main Registers
 * @ingroup RTCMacros
 * @brief RTC's Main Register's Addresses
 * @{
 */
#define RTC_ADDR_REG 0x70 /**< @brief RTC's Address Register */
#define RTC_DATA_REG 0x71 /**< @brief RTC's Data Register */
/** @} */

/** 
 * @defgroup RTCRegisters RTC's 1-byte registers
 * @ingroup RTCMacros
 * @brief RTC has 64 1B Registers
 * @details Those are:
 * - First 10 -> Time-related functionality
 * - Following 4 -> Control of RTC
 * - Last 50 -> Arbitrary purposes
 * @{
 */
#define YEAR_ADDR 0x9     /**< @brief Year Register's Address */
#define MONTH_ADDR 0x8    /**< @brief Month Register's Address */
#define DAY_ADDR 0x7      /**< @brief Day Register's Address */
#define WEEKDAY_ADDR 0x6  /**< @brief Weekday Register's Address */
#define HOURS_AL_ADDR 0x5 /**< @brief Hours Alarm Register's Address */
#define HOURS_ADDR 0x4    /**< @brief Hours Register's Address */
#define MINS_AL_ADDR 0x3  /**< @brief Minutes Alarm Register's Address */
#define MINS_ADDR 0x2     /**< @brief Minutes Register's Address */
#define SECS_AL_ADDR 0x1  /**< @brief Seconds Alarm Register's Address */
#define SECS_ADDR 0x0     /**< @brief Seconds Register's Address */

#define AL_DONTCARE BIT(7)|BIT(6) /**< @brief Don't-Care value for Alarm Registers - match any value in the register (alarms in multiple times of day) */
/** @} */

/** 
 * @defgroup RTCA RTC's Control Register A
 * @ingroup RTCMacros
 * @brief REGISTER 10
 * @{
 */
#define RA_ADDR 0xA   /**< @brief Register A's address */
#define RA_UIP BIT(7) /**< @brief Set to 1 - update in progress. @details It's set to 1 244 µs before an update and reset immediatly afterwards */
#define RA_RS3 BIT(3) /**< @brief Rate selector - for periodic interrupts and square-wave output */
#define RA_RS2 BIT(2) /**< @brief Rate selector - for periodic interrupts and square-wave output */
#define RA_RS1 BIT(1) /**< @brief Rate selector - for periodic interrupts and square-wave output */
#define RA_RS0 BIT(0) /**< @brief Rate selector - for periodic interrupts and square-wave output */
/** @} */

/** 
 * @defgroup RTCB RTC's Control Register B
 * @ingroup RTCMacros
 * @brief REGISTER 11
 * @details Do not change bits 0,1,2 - may interfere with OS
 * @{
 */
#define RB_ADDR 0xB     /**< @brief Register B's address */
#define RB_SET BIT(7)   /**< @brief Set to 1 to inhibit updates of time/data registers */
#define RB_PIE BIT(6)   /**< @brief Set to 1 to enable periodic interrupts. @details With a period between 122 µs and 0.5s as determined by RS0-RS3 in Register A  */
#define RB_AIE BIT(5)   /**< @brief Set to 1 to enable alarm interrupts.  */
#define RB_UIE BIT(4)   /**< @brief Set to 1 to enable update interrupts */
#define RB_SQWE BIT(3)  /**< @brief Set to 1 to enable square-wave generation */
#define RB_DM BIT(2)    /**< @brief 1 - time, alarm and date registers in binary. 0 - in BCD */
#define RB_2412 BIT(1)  /**< @brief 1 - hours from 0 to 23. 0 - from 1 to 12 */
#define RB_DSE BIT(0)   /**< @brief 1 - Daylight Saving Time. 0 - disable. Useless: old US DST */
/** @} */

/** 
 * @defgroup RTCC RTC's Control Register C
 * @ingroup RTCMacros
 * @brief REGISTER 12
 * @details Bits 0,1,2,3 at 0
 * @{
 */
#define RC_ADDR 0xC     /**< @brief Register C's address */
#define RC_IRQF BIT(7)  /**< @brief IRQ line active */
#define RC_PF BIT(6)    /**< @brief Flag - Periodic interrupt pending */
#define RC_AF BIT(5)    /**< @brief Flag - Alarm interrupt pending */
#define RC_UF BIT(4)    /**< @brief Flag - Update interrupt pending */
/** @} */

/** 
 * @defgroup RTCD RTC's Control Register D
 * @ingroup RTCMacros
 * @brief REGISTER 13
 * @details Bits 0,1,2,3 at 0
 * @{
 */
#define RD_ADDR 0xD   /**< @brief Register D's address */
#define RD_VRT BIT(7) /**< @brief Valid RAM/time - set to 0 when internal lithium battery runs out of energy. RTC readings questionable */
/** @} */

/** @} */


#endif // RTC_MACROS_H

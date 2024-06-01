#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

#include <stdint.h>


/** 
 * @brief Declaration of the rtc_data struct
 * @details Provides encapsulation by only declaring the struct and it's alias (rtc_data_t) in the rtc.h file. The actual implementation is out of reach of the rest of the project, in the rtc.c file.
*/
struct rtc_data;
/** @brief Alias for struct rtc_data type  */
typedef struct rtc_data rtc_data_t;

/**
 * @brief Configures the RTC's initial state
 * @details Initializes the rtc_binary member variable (is_rtc_binary()). Initializes the current_time (read_current_time()). Sets hourly alarm for background-image changing (set_hourly_alarm()).
 * 
 * @return 0 upon success, non-zero otherwise
*/
int configure_rtc();

/**
 * @brief Subscribes and enables RTC interrupts
 * 
 * @param bit_no address of memory to be initialized with the bit number to be set in the mask returned upon an interrupt
 * @return 0 upon success and non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes RTC interrupts
 *
 * @return 0 upon success and non-zero otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief RTC's Interrupt Handler
 * @details Only 1 type of interrupt is being handled, because the others aren't activated in configure_rtc()
*/
void rtc_ih(void);

/**
 * @brief Handler for Alarm Interrupts
 * @details Updates the current time and changes the sky background according to the real-time sky
 * 
 * @return 0 upon success and non-zero otherwise
*/
int handle_alarm_int();

/**
 * @brief Reads the value in the specified RTC internal register
 * @details Loads the RTC Address Register with the register from reg_addr. Reads the RTC Data Register and stores the value in output
 * 
 * @param reg_addr Register to be read
 * @param output Pointer to a uint8_t variable to store the value read
 * @return 0 upon success and non-zero otherwise
*/
int rtc_read(uint8_t reg_addr, uint8_t *output);

/**
 * @brief Writes the specified value in the specified RTC internal register
 * @details Loads the RTC Address Register with the register from reg_addr. Loads that address with the value from byte
 * 
 * @param reg_addr Register to be read
 * @param byte uint8_t value to written to the register in reg_addr
 * @return 0 upon success and non-zero otherwise
*/
int rtc_write(uint8_t reg_addr, uint8_t byte);

/**
 * @brief Checks if the RTC has its registers' values in binary format or BCD.
 * @details updates the rtc_binary global variable
 * 
 * @return 0 upon success and non-zero otherwise
*/
int is_rtc_binary();

/**
 * @brief Converts BCD value into decimal
 * @details Called for every value read from a time register to ensure that it is stored in decimal form. RTC 1B registers can be in BCD data-format. Conversion is done according to the following example:
 * Decimal: 12
 * BCD: 0001 0010 -> 1 * 10^1 + 2 * 10^0
 * @param bcd_num uint8_t BCD value to be converted
 * 
 * @return BCD value in binary
*/
uint8_t to_decimal(uint8_t bcd_num);


/**
 * @brief Reads the current time in the Real-Time Clock
 * @details Software implementation to avoid race conditions while reading the time. Reads all the time registers in the RTC (read_time_regs()) - TWICE - and compares the values (equal_reads()): 
 * - If the values are the same, there weren't any updates while reading that might cause significant value changes.
 * - If the values are different, it means that there was an update while reading and it tries to read the values again.
 * - After 10 times, if it wasn't successful, it gives up.
 * 
 * @returns 0 upon success and non-zero otherwise
*/
int read_current_time();

/**
 * @brief Reads all the time registers
 * @details The time registers are: seconds, minutes, hours, weekday, day, month, year
 * @param read Pointer to a struct rtc_data_t variable that stores the values read
 * 
 * @returns 0 upon success and non-zero otherwise
*/
int read_time_regs(rtc_data_t *read);

/**
 * @brief Compares 2 rtc_data_t variables for equality
 * @param read1, read2 The two time structs to be compared
 * 
 * @returns true if every struct member is equal, false otherwise
*/
bool equal_reads(rtc_data_t *read1, rtc_data_t *read2);

/**
 * @brief Activates alarm interrupts and sets the alarm to generate an interrupt every hour
 * @details The sky's background will change every couple of hours, so there's a need to keep track of the current hour
 * 
 * @return 0 upon success, 1 otherwise
*/
int set_hourly_alarm();


#endif // RTC_H

#include "../../objects/object_controllers/sky_controller.h"
#include "rtc_macros.h"
#include "rtc.h"


/**
 * @struct rtc_data
 * @brief Implementation of the rtc_data struct to hold real time clock data.
 * @details It includes seconds, minutes, hours, weekday, day, month, and year.
 * 
 * @var rtc_data::seconds
 * The seconds component of the time.
 * 
 * @var rtc_data::minutes
 * The minutes component of the time.
 * 
 * @var rtc_data::hours
 * The hours component of the time.
 * 
 * @var rtc_data::weekday
 * The current day of the week.
 * 
 * @var rtc_data::day
 * The current day of the month.
 * 
 * @var rtc_data::month
 * The current month.
 * 
 * @var rtc_data::year
 * The current year.
 */
struct rtc_data {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint8_t year;
};


static bool rtc_binary;
static rtc_data_t current_time;

int configure_rtc() {
  if (is_rtc_binary()) {printf("Error: reading data format!\n"); return 1;}
  if (read_current_time()) {printf("Error: reading current time!\n"); return 1;}
  if (set_hourly_alarm()) {printf("Error: setting hourly alarm!\n"); return 1;}

  return 0;
}


static int rtc_hookId = RTC_IRQ;
int rtc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = rtc_hookId;

  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hookId);
}
int rtc_unsubscribe_int() {
  return sys_irqrmpolicy(&rtc_hookId);
}

void rtc_ih(void) {
  uint8_t cause; // Cause of the interrupt

  if (rtc_read(RC_ADDR, &cause)) {printf("Error while reading from %x RTC address\n", RC_ADDR);}

  if (cause & RC_AF) {
    if (handle_alarm_int()) {
      printf("Error while handling the RTC's alarm interrupt!\n");
    }
  }
}

int handle_alarm_int() {
  if (read_current_time()) {
    printf("%s: Error while reading current time!\n", __func__);
    return 1;
  }

  return 0;
}


int rtc_read(uint8_t reg_addr, uint8_t *output) {
  if (sys_outb(RTC_ADDR_REG, reg_addr)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
	if (util_sys_inb(RTC_DATA_REG, output)) {
    printf("Error while reading value from %x port\n", RTC_DATA_REG);
    return 1;
  }

  return 0;
}

int rtc_write(uint8_t reg_addr, uint8_t byte) {
  if (sys_outb(RTC_ADDR_REG, reg_addr)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
	if (sys_outb(RTC_DATA_REG, byte)) {
    printf("Error while writing value to %x port", RTC_DATA_REG);
    return 1;
  }

  return 0;
}


uint8_t to_decimal(uint8_t bcd_num) {
  return ((bcd_num >> 4) * 10) + (bcd_num & 0x0F);
}

int is_rtc_binary() {
  uint8_t res_rb;

  if (sys_outb(RTC_ADDR_REG, RB_ADDR)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
  if (util_sys_inb(RTC_DATA_REG, &res_rb)) {
    printf("Error while writing value to %x port", RTC_DATA_REG);
    return 1;
  }

  rtc_binary = res_rb & RB_DM;
  return 0;
}


int read_current_time() {
  uint8_t tries = MAX_READS;
  rtc_data_t read1, read2;

  // Keep going until both reads are the same
  while (tries--) {
    if (read_time_regs(&read1)) {printf("Error during first rtc read\n"); return 1;}
    if (read_time_regs(&read2)) {printf("Error during second rtc read\n"); return 1;}
    if (equal_reads(&read1, &read2)) {
      current_time = read1;
      return 0;
    }
  }

  printf("%s Error: Ran for %d tries and didn't succeed!\n", __func__, MAX_READS);
  return 1;
}

int read_time_regs(rtc_data_t *read) {
  uint8_t info;

  if (rtc_read(SECS_ADDR, &info)) {
    printf("%s: Error while reading seconds\n", __func__);
    return 1;
  }
  read->seconds = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(MINS_ADDR, &info)) {
    printf("%s: Error while reading minutes\n", __func__);
    return 1;
  }
  read->minutes = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(HOURS_ADDR, &info)) {
    printf("%s: Error while reading hours\n", __func__);
    return 1;
  }
  read->hours = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(WEEKDAY_ADDR, &info)) {
    printf("%s: Error while reading weekday\n", __func__);
    return 1;
  }
  read->weekday = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(DAY_ADDR, &info)) {
    printf("%s: Error while reading day\n", __func__);
    return 1;
  }
  read->day = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(MONTH_ADDR, &info)) {
    printf("%s: Error while reading month\n", __func__);
    return 1;
  }
  read->month = (rtc_binary) ? info : to_decimal(info);

  if (rtc_read(YEAR_ADDR, &info)) {
    printf("%s: Error while reading year\n", __func__);
    return 1;
  }
  read->year = (rtc_binary) ? info : to_decimal(info);

  return 0;
}

bool equal_reads(rtc_data_t *read1, rtc_data_t *read2) {
  return read1->seconds == read2->seconds && 
         read1->minutes == read2->minutes &&
         read1->hours == read2->hours &&
         read1->weekday == read2->weekday &&
         read1->day == read2->day &&
         read1->month == read2->month &&
         read1->year == read2->year;
}


int set_hourly_alarm() {
  uint8_t rb_status;

  if (rtc_read(RB_ADDR, &rb_status)) {printf("Error while reading Reg B\n"); return 1;}
  // Activate alarm interrupts
  rb_status |= RB_AIE;
  if (rtc_write(RB_ADDR, rb_status)) {printf("Error while writing to Reg B\n"); return 1;}
  
  // Set hourly alarm
  if (rtc_write(SECS_AL_ADDR, 0)) { // Everytime seconds reach 0
    printf("Error while writing to SECS_AL reg\n"); 
    return 1;
  }
  if (rtc_write(MINS_AL_ADDR, 0)) { // Everytime minutes reach 0
    printf("Error while writing to MINS_AL reg\n"); 
    return 1;
  }
  if (rtc_write(HOURS_AL_ADDR, AL_DONTCARE)) { // Every hour
    printf("Error while writing to HOURS_AL reg\n"); 
    return 1;
  }

  return 0;
}


uint8_t get_current_hour() {
  return current_time.hours;
}

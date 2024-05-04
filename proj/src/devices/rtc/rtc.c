#include <lcom/lcf.h>

#include <stdint.h>

#include "rtc.h"
#include "rtc_macros.h"



static bool rtc_binary;
static rtc_data_t current_time;

int configure_rtc() {
  if (is_rtc_binary()) {print("%s: Error while reading data format!", __func__); return 1;}
  if (get_current_time()) {print("%s: Error while reading data format!", __func__); return 1;}

  return 0;
}

int is_rtc_binary() {
  uint8_t res_rb;

  if (sys_outb(RTC_ADDR_REG, RB_ADDR)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
  if (sys_outb(RTC_DATA_REG, &res_rb)) {
    print("Error while writing value to %x port", RTC_DATA_REG);
    return 1;
  }

  rtc_binary = res_rb & RB_DM;
  return 0;
}


static int rtc_hookId = RTC_IRQ;
int rtc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = rtc_hookId;

  // No need for IRQ_EXCLUSIVE - By default MINIX3 doesn't have driver for RTC
  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hookId);
}
int rtc_unsubscribe_int() {
  return sys_irqrmpolicy(&rtc_hookId);
}

void rtc_ih(void) {
  uint8_t cause; // Cause of the interrupt

  if (read_rtc(RC_ADDR, &cause)) {printf("Error while reading from %x RTC address\n", RC_ADDR);}

  if (cause & RC_UF) handle_update_int();
  if (cause & RC_AF) handle_alarm_int();
  if (cause & RC_PF) handle_periodic_int();
}


int read_rtc(uint8_t reg_addr, uint8_t *output) {
  if (sys_outb(RTC_ADDR_REG, reg_addr)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
	if (util_sys_inb(RTC_DATA_REG, output)) {
    print("Error while reading value from %x port", RTC_DATA_REG);
    return 1;
  }

  return 0;
}

int write_rtc(uint8_t reg_addr, uint8_t byte) {
  if (sys_outb(RTC_ADDR_REG, reg_addr)) {
    printf("Error while writing address to %x port\n", RTC_ADDR_REG); 
    return 1;
  }
	if (sys_outb(RTC_DATA_REG, byte)) {
    print("Error while writing value to %x port", RTC_DATA_REG);
    return 1;
  }

  return 0;
}

/*
int is_rtc_updating(bool *isUpdating) {
  uint8_t update_st;

  if (read_rtc(RA_ADDR, &update_st)) {
    printf("Error while reading from %x RTC address\n", RA_ADDR); 
    return 1;
  }
  
  isUpdating = update_st & RA_UIP;
  return 0;
}
*/

// RTC 1B registers can be in BCD data-format
// Decimal: 12
// Binary: 1100 - how it's stored
// BCD: 0001 0010 - 1 * 10^1 + 2 * 10^0
uint8_t to_decimal(uint8_t bcd_num) {
  return (bcd_num >> 4 * 10) + (bcd_num & 0x0F);
}


int get_current_time() {
  uint8_t tries = 10;
  rtc_data_t read1, read2;

  // Keep going until both reads are the same
  while (tries--) {
    if (read_time_regs(&read1)) {print("Error during first rtc read\n"); return 1;}
    if (read_time_regs(&read2)) {print("Error during second rtc read\n"); return 1;}
    if (equal_reads(&read1, &read2)) {
      current_time = read1;
      return 0;
    }
  }

  return 1;
}

int read_time_regs(rtc_data_t *read) {
  uint8_t info;

  if (read_rtc(SECS_REG, &info)) {
    printf("%s: Error while reading seconds\n", __func__);
    return 1;
  }
  current_time.seconds = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(MINS_REG, &info)) {
    printf("%s: Error while reading minutes\n", __func__);
    return 1;
  }
  current_time.minutes = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(HOURS_REG, &info)) {
    printf("%s: Error while reading hours\n", __func__);
    return 1;
  }
  current_time.hours = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(WEEKDAY_REG, &info)) {
    printf("%s: Error while reading weekday\n", __func__);
    return 1;
  }
  current_time.weekday = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(DAY_REG, &info)) {
    printf("%s: Error while reading day\n", __func__);
    return 1;
  }
  current_time.day = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(MONTH_REG, &info)) {
    printf("%s: Error while reading month\n", __func__);
    return 1;
  }
  current_time.month = (rtc_binary) ? info : to_decimal(info);

  if (read_rtc(YEAR_REG, &info)) {
    printf("%s: Error while reading year\n", __func__);
    return 1;
  }
  current_time.year = (rtc_binary) ? info : to_decimal(info);

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


// Getters
bool get_RTC_binary() {
  return rtc_binary;
}
rtc_data_t get_current_time() {
  return current_time;
}

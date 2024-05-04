#ifndef RTC_H
#define RTC_H

#include <stdint.h>


typedef struct {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t weekday;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} rtc_data_t;

int rtc_subscribe_int(uint8_t *bit_no);
int rtc_unsubscribe_int();

#endif // RTC_H
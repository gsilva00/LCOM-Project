#ifndef RTC_H
#define RTC_H

#include <stdint.h>


int rtc_subscribe_int(uint8_t *bit_no);
int rtc_unsubscribe_int();

#endif // RTC_H
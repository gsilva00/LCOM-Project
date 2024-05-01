#include <lcom/lcf.h>

#include <stdint.h>

#include "rtc_macros.h"

static int rtc_hookId = RTC_IRQ;

#include "rtc.h"

int rtc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = rtc_hookId;
  return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &rtc_hookId);
}

int rtc_unsubscribe_int() {
  return sys_irqrmpolicy(&rtc_hookId);
}


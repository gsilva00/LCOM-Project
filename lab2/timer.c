#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(freq < 19 || freq > TIMER_FREQ || timer < 0 || timer > 2) return 1;
  else{
    __uint32_t rb_cmd = TIMER_RB_CMD|TIMER_RB_COUNT_| TIMER_RB_SEL(timer);
    uint8_t inf = 0;
    sys_outb(TIMER_CTRL, rb_cmd);
    util_sys_inb(TIMER_0 + timer, &inf);
    inf &= 0x0f;
    if(timer == 0) inf |= TIMER_LSB_MSB;
    else if(timer == 1) inf |= TIMER_LSB_MSB | TIMER_SEL1;
    else inf |= TIMER_LSB_MSB | TIMER_SEL2;
    uint16_t div = (uint16_t)(TIMER_FREQ/freq);
    sys_outb(TIMER_CTRL, inf);
    uint8_t lsb;
    uint8_t msb;
    util_get_LSB(div, &lsb);
    util_get_MSB(div, &msb);
    sys_outb(TIMER_0 + timer, lsb);
    sys_outb(TIMER_0 + timer, msb);
    return 0;
  }
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(st == NULL || timer > 2 || timer < 0) return 1;
  else{
    __uint32_t rb_cmd = TIMER_RB_CMD|TIMER_RB_COUNT_| TIMER_RB_SEL(timer);
    sys_outb(TIMER_CTRL, rb_cmd);
    if(util_sys_inb(TIMER_0 + timer, st)) return 1;
    else return 0;
  }
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  if(timer > 2 || timer < 0) return 1;
  else if(field != tsf_all && field != tsf_base && field != tsf_initial && field != tsf_mode) return 1;
  else{
    union timer_status_field_val data;
    timer_get_conf(timer, &st);
    switch(field){
      case tsf_all:
        data.byte = st;
        break;
      case tsf_initial:
        st = st << 2;
        st = st >> 6;
        data.in_mode = st;
        break;
      case tsf_base:
        st &= 0xfc;
        data.bcd = st;
        break;
      default:
        st = st << 4;
        st = st >> 5;
        data.count_mode = st;
    }
    timer_print_config(timer, field, data);
    return 0;
  }
}

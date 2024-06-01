#include <lcom/lcf.h>

#include <stdint.h>

#include <lcom/timer.h>
#include "aux_timer.h"
#include "i8254.h"


// Arbitrary value -> it will be the mask's bit before being timer's hook_id
static int timer_hookId = TIMER0_IRQ;
static int timer_intCounter = 0;

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) {printf("Pointer to store bit_no points to NULL!\n"); return 1;}
  *bit_no = timer_hookId;
  return (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hookId));
}
int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&timer_hookId);
}

void (timer_int_handler)() {
  timer_intCounter++;
}

int get_timer_intCounter() {
  return timer_intCounter;
}
void set_timer_intCounter(int newTime) {
  timer_intCounter = newTime;
}

#include "timer.h"

int hookid_timer = 0;
int (timer_subscribe)(uint8_t * bit_no_timer){
  if(bit_no_timer == NULL){
    return 1;
  }
  *bit_no_timer = BIT(hookid_timer);
  if(sys_irqsetpolicy(IRQ_LINE_TIMER0, IRQ_REENABLE, &hookid_timer) ){
    return 1;
  }
  return 0;
}

int (timer_unsubscribe)(){
  if(sys_irqrmpolicy(&hookid_timer)){
    return 1;
  }
  return 0;
}

extern int counter_timer;

void (timer_ih)(){
  counter_timer++;
 // printf(" %d",counter_timer);
}


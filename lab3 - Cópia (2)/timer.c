#include "timer.h"
#include "itimer.h"

int hookid_timer = 0;
int(timer_subscribe)(uint8_t *bit_no){
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(hookid_timer);
  if(sys_irqsetpolicy(IRQLINE_TIMER, IRQ_REENABLE,&hookid_timer)){
    return 1;
  }

  return 0;
}

int(timer_unsubscribe)(){
  if(sys_irqrmpolicy(&hookid_timer)){
    return 1;
  }
  return 0;
}

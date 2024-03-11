#include <lcom/lcf.h>
#include <lcom/timer.h>

#include "i8254.h"
extern uint8_t counter;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(timer > 2){
    return 1;
  }
  if(freq < 19 || freq > TIMER_FREQ){
    return 1;
  }
  //ler antes de tudo o status do timer 0?
  uint8_t status = 0;
  if(timer_get_conf(timer,&status)) {return 1;}
  //control word não pode mudar s ultimos 4 bits(mode 3 e bcd/-binary counting) 0x0000 1111
  uint8_t parteqnaomuda = status & 0x0f;
  printf("%x ",status);
  printf("%x ",parteqnaomuda);
  // LSB antes de MSB
  uint32_t cmd = timer << 6 | TIMER_LSB_MSB | parteqnaomuda;
  printf("%x",cmd);
  //mudar a frequencia a que o timer 0 gera interrupts escrevendo control word em 0x43 antes de acessar os timers
  if(sys_outb(TIMER_CTRL,cmd)){
    return 1;
  }
  //queremos generate a time base with a given frequency in Hz, por isso escrevemos a base no timer0
  //timer base é o tempo que demora a completar um ciclo completo (Periodo T)
  //como qqueremos mudar o periodo: (T final) = 1/(F final) mas * freq atual       
  uint16_t periodo = TIMER_FREQ / freq;  
  //colocar divisor no timer0
  int port = TIMER_0 + timer;
  uint8_t parte1 = 0; 
  util_get_LSB(periodo,&parte1);
  uint8_t parte2 = 0; 
  util_get_MSB(periodo,&parte2);
  if(sys_outb(port,parte1)){return 1;}
  if(sys_outb(port,parte2)){return 1;}

  return 0;
}


//subscrever timer0 (hook_id de timer é normalmente 0 mas não foi predefinido no i8254.h)
int hook_id_timer = 0;  //pomos como variavel global porque vai nos dar jeito para outras funções


int (timer_subscribe_int)(uint8_t *bit_no) {
   if(bit_no == NULL){
    return 1;
   }
  *bit_no = BIT(hook_id_timer);     //ele cria um 1 no 0º posicao do line(?)
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id_timer)){
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id_timer)){ //<<<< usado hook_id do timer aqui tambem para sabermos qual id elimar sem usar no parametro
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

#include "keyboard.h"
#include "utils.h"

extern uint8_t bit_no;
extern  uint8_t bit_no_timer;
extern uint8_t n;
int hookid_kbd = 1;

int (enable_interrupt)(){
  printf("hello");
  if(utils_sys_outb(CONTROL_REG, COMAND_TO_READ_COMAND_BYTE)){
    printf("1");
    return 1;
  }
  printf("aaa");
  uint8_t command_byte = 0;
  if(utils_sys_inb(OUT_BUF, &command_byte)){
    printf("2");
    return 1;
  }
  printf("bb");
  uint8_t novo_command = command_byte | ENABLE_INTERRUPT_OBF;

  if(utils_sys_outb(CONTROL_REG, COMAND_TO_WRITE_COMAND_BYTE)){
    printf("3");
    return 1; 
  }
  printf("cc");
  if(utils_sys_outb(OUT_BUF, novo_command)){
    printf("4");
    return 1;
  }

  printf("dd");
  if(utils_sys_outb(CONTROL_REG, COMAND_TO_READ_COMAND_BYTE)){
    printf("1");
    return 1;
  }
  command_byte = 0;
  if(utils_sys_inb(OUT_BUF, &command_byte)){
    printf("2");
    return 1;
  }

  if(command_byte & ENABLE_INTERRUPT_OBF){
    printf("esta bem posto");
  }else{
    printf("esta mal");
  }

  return 0;

}


int (kbd_subscribe)(uint8_t * bit_no){
  if(bit_no == NULL){
    return 1;
  }
  *bit_no = BIT(hookid_kbd);
  if(sys_irqsetpolicy(IRQLINE_KBD,IRQ_REENABLE|IRQ_EXCLUSIVE,&hookid_kbd)){
    return 1;
  }

  return 0;
}


int (kbd_unsubscribe)(){
  if(sys_irqrmpolicy(&hookid_kbd)){
    return 1;
  }
  return 0;
}

int (verify_error)(){
  uint8_t status = 0;
  int tries = 20;
  while (tries){
    if(utils_sys_inb(STATUS_REG, &status)){
        printf("erro ao ler status");
        return 1;
      }

      if(status & OBF){
        tries = 1;
        break;
      }else{
        tickdelay(micros_to_ticks(DELAY_US));
        continue;
      }
    tries--;
  }
  if(tries == 0){
    printf("chegou ao das tries status");
    return 1;
  }
    if((status & PARITY) | (status & TIMEOUT)){
      printf("erro de parity ou timeout");
      return 1;
    }
    return 0;

}


void (kbc_ih)(){
  int ipc_status, r;
  message msg;

  uint8_t scancode = 0;
  uint8_t bytes[2];
  uint8_t time = n;
  uint8_t counter_time = 0;

  while (time) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & bit_no_timer) {
            counter_time++;

            if(counter_time == 60){
              time--;
              printf("time: %d",time);
              counter_time = 0;
            }
          }
          
          if (msg.m_notify.interrupts & bit_no) { // subscribed interrupt
              if(verify_error()){
                printf("erro no status");
                continue;
              }

              time = n;
              counter_time = 0;

              if(utils_sys_inb(OUT_BUF,&scancode)){
                printf("erro no ler obf");
                continue;
              }

              bool make = 1;
              if(scancode & BREAKCODE){
                make = 0;
              }

              bytes[0] = scancode;

              if(kbd_print_scancode(make ,1,&scancode)){
                continue;
              }

              if(ESC_BREAKCODE == scancode){
                time = 0;
              }
          }
          break;
        default:
          break;
      }
    }
  }
}

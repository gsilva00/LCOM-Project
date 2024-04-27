#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>

#include "keyboard.h"
#include "i8042.h"

#include "timer.h"

#include "utils.h"

uint32_t count = 0;

uint8_t bit_no = 0;
uint8_t bit_no_timer = 0;
uint8_t n = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  //dar print de make code e break code do KBC
  //verifcar se dá erro na entrega
  //deve terminar quando lê 0x81 ou seja o break code do ESC
  //trabalhar com 2 bytes scancodes. 1º bite é 0xE0

  //temos que dar subscribe ao kbc paraa sabermos e dá interrupt para depois ler o OUT_BUF
  //para prevenir que General interrupt Handler do Minix intervenha, vamos no setpolicy colocar IRQ_REENABLE|IRQ_EXCLUSIVE
  if(kbd_subscribe(&bit_no)){
    return 1;
  }

  int ipc_status, r;
  message msg;

  uint8_t scancode = 0;
  uint8_t bytes[2];

  bool done = false;

  while (!done) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             // hardware interrupt notification
          if (msg.m_notify.interrupts & bit_no) { // subscribed interrupt

              if(verify_error()){
                printf("erro no status");
                continue;
              }

              if(utils_sys_inb(OUT_BUF,&scancode)){
                printf("erro no ler obf");
                continue;;
              }

              bool make = 1;
              if(scancode & BREAKCODE){
                make = 0;
              }

              bytes[0] = scancode;

              if(kbd_print_scancode(make ,1,&scancode)){
                return 1;
              }

              if(ESC_BREAKCODE == scancode){
                done = true;
              }

          }
          break;
        default:
          break;
      }
    }
  }

  if(kbd_print_no_sysinb(count)){
    return 1;
  }

  if(kbd_unsubscribe()){
    return 1;
  }
  return 0;
}

int(kbd_test_poll)() {
  uint8_t scancode = 0;
  uint8_t bytes[2];

  bool done = false;

  while (!done) {
              if(verify_error()){
                printf("erro no status");
                continue;
              }

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
                return 1;
              }

              if(ESC_BREAKCODE == scancode){
                break;
              }

          }

  if(kbd_print_no_sysinb(count)){
    return 1;
  }

  if(enable_interrupt()){
    printf("nao consegui dar enable interrupt :(");
    return 1;
  }


  return 0;
}

int(kbd_test_timed_scan)(uint8_t n_) {

  if(timer_subscribe(&bit_no_timer)){
    return 1;
  }

  if(kbd_subscribe(&bit_no)){
    return 1;
  }

  n = n_;
  
  kbc_ih();

  if(kbd_unsubscribe()){
    return 1;
  }

  if(timer_unsubscribe()){
    return 1;
  }

  return  0;
}

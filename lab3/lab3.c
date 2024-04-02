#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "kbc.h"
#include <lcom/timer.h>

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

uint8_t timer_bit;
uint8_t kbd_bit;
extern uint8_t output;
extern uint32_t counter;
extern uint32_t counter_timer;

int(kbd_test_scan)() {
  int ipc_status, r;
  message msg;
  if(kbd_subscribe_int(&kbd_bit) != 0){
    return 1;
  }
  while(output != BREAKCODE_ESC) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:				
                if (msg.m_notify.interrupts & kbd_bit) {
                  kbc_ih();
                  if(output == 0xE0){
                    if(kbd_print_scancode(!(output & BREAKCODE), 2, &output) != 0){
                      return 1;
                    }
                  }else{
                    if(kbd_print_scancode(!(output & BREAKCODE), 1, &output) != 0){
                      return 1;
                    }
                  }
                }
                break;
            default:
                break;
        }
    } else {}
  }
  if(kbd_unsubscribe_int() != 0){
    return 1;
  }
  kbd_print_no_sysinb(counter);
  return 0;
}

int(kbd_test_poll)() {
  while(output != BREAKCODE_ESC) {
    kbc_read_output(OUTBUF_REG, &output);
    if(output == 0xE0){
      if(kbd_print_scancode(!(output & BREAKCODE), 2, &output) != 0){
        return 1;
      }
    }else{
      if(kbd_print_scancode(!(output & BREAKCODE), 1, &output) != 0){
        return 1;
      }
    }
  }
  if(kbd_reset_int()){
    return 1;
  }
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r;
  uint8_t time = 0;
  message msg;
  if(timer_subscribe_int(&timer_bit) != 0){
    return 1;
  }
  if(kbd_subscribe_int(&kbd_bit) != 0){
    return 1;
  }
  while(output != BREAKCODE_ESC && time < n) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:	
                if (msg.m_notify.interrupts & timer_bit) {
                  timer_int_handler();
                  if(counter_timer % 60 == 0){
                    time++;
                  }
                }			
                if (msg.m_notify.interrupts & kbd_bit) {
                  kbc_ih();
                  if(output == 0xE0){
                    if(kbd_print_scancode(!(output & BREAKCODE), 2, &output) != 0){
                      return 1;
                    }
                  }else{
                    if(kbd_print_scancode(!(output & BREAKCODE), 1, &output) != 0){
                      return 1;
                    }
                  }
                  time = 0;
                }
                break;
            default:
                break;
        }
    } else {}
  }
  if(kbd_unsubscribe_int() != 0){
    return 1;
  }
  if(timer_unsubscribe_int() != 0){
    return 1;
  }
  kbd_print_no_sysinb(counter);
  return 0;
}

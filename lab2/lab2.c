#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

#include "aux_timer.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t stReceiver;
  
  if (timer_get_conf(timer, &stReceiver)) return 1;
  if (timer_display_conf(timer, stReceiver, field)) return 1;  

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (timer_set_frequency(timer, freq)) return 1;

  return 0;
}

int(timer_test_int)(uint8_t time) {
  int ipc_status, r;
  message msg;
  
  // See notes below:
  uint8_t bit_no;
  if (timer_subscribe_int(&bit_no)) return 1;
  uint32_t irq_set = BIT(bit_no);

  while (time) { 
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification 
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
            timer_int_handler();
            if (!(get_int_counter() % 60)) { // See notes below:
              timer_print_elapsed_time();
              time--;
            }
          }
          break;
        default:
          // no other notifications expected: do nothing
          break;
      }
    } 
    else { // received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
  if (timer_unsubscribe_int()) return 1;
  return 0;
}

/*
NOTES:
<About timer_test_int()>
Note on bit_no: 
- Arbitrary bit that the kernel activates when the corresponding subscribed interrupt is pending. The GIH (Generic Interrupt Handler) will send a message to the DD (Device Driver) with the bit_no set to 1 on m_notify.interrupts.
- This allows the kernel to send a single message to notify a process of the occurrence of several interrupts on different IRQ lines.

Note on the number 60:
- Timer runs at 60Hz -> 60 clock cycles per second, 60 interrupts per second, need to print message every second, so need to check when it is a multiple of 60 -> get_int_counter() % 60 == 0
*/

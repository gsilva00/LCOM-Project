#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"


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
  int ipc_status, r;
  message msg;

  uint8_t scancode_size = 1;
  
  uint8_t bit_no;
  if (kbc_subscribe_int(&bit_no)) return 1;
  uint32_t irq_set = BIT(bit_no);

  while (get_scancode() != BREAKCODE_ESC) { // ESC breakcode found
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { // subscribed interrupt
            kbc_ih();
            
            uint8_t scancode = get_scancode();
            
            if (scancode == SC_MSB1 || scancode == SC_MSB2) {
              scancode_size = 2;
              continue;
            }
            else {
              if (kbd_print_scancode(!(scancode & BREAKCODE), scancode_size, &scancode)) return 1;
              scancode_size = 1;
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
  
  if (kbc_unsubscribe_int()) return 1;
  if (kbd_print_no_sysinb(get_sysinb_count())) return 1;
  return 0;
}

int(kbd_test_poll)() {
  

  //if (kbd_print_scancode()) return 1;

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

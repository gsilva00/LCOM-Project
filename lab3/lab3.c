#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include <lcom/timer.h>
#include "aux_timer.h"
#include "keyboard.h"
#include "kbc.h"
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
  
  uint8_t bit_no;
  if (kbc_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);

  while (get_scancode() != BREAKCODE_ESC) { // ESC breakcode found
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & kbc_int_bit) { // subscribed interrupt
            kbc_ih();
            
            uint8_t scancode = get_scancode();
            if (kbd_print_scancode(
              !(scancode & BREAKCODE), 
              (scancode == SC_MSB1) ? 2 : 1, 
              &scancode
            )) return 1;
            
            // See comments below
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
  uint8_t scancode = get_scancode();

  while (scancode != BREAKCODE_ESC) {
    scancode = get_scancode();
    
    if (kbc_read_outbuf(KBC_OUTBUF, &scancode)) return 1;
    
    if(kbd_print_scancode(
      !(scancode & BREAKCODE), 
      scancode == SC_MSB1 ? 2 : 1, 
      &scancode
    )) return 1;   
  }
  
  if (reset_keyboard()) return 1;
  if (kbd_print_no_sysinb(get_sysinb_count())) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r;
  message msg;
  
  uint8_t bit_no;
  if (kbc_subscribe_int(&bit_no)) return 1;
  uint32_t kbc_int_bit = BIT(bit_no);
  if (timer_subscribe_int(&bit_no)) return 1;
  uint32_t timer0_int_bit = BIT(bit_no);

  int time_passed = 0;
  while (get_scancode() != BREAKCODE_ESC && time_passed < n) { // ESC breakcode found
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
        // Higher priority devices' interrupts first to be handled
          if (msg.m_notify.interrupts & timer0_int_bit) { // subscribed timer interrupt
            timer_int_handler();
            if (get_timer_intCounter() % 60 == 0) time_passed++;
          }
          if (msg.m_notify.interrupts & kbc_int_bit) { // subscribed kbc interrupt
            kbc_ih();
            
            uint8_t scancode = get_scancode();
            if (kbd_print_scancode(
              !(scancode & BREAKCODE), 
              (scancode == SC_MSB1) ? 2 : 1, 
              &scancode
            )) return 1;
            
            time_passed = 0;
            set_timer_intCounter(0);
            // See comments below
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
  if (timer_unsubscribe_int()) return 1;
  return 0;
}

/* NOTES: 
<kbd_test_scan()>
.Keyboard Interrupts: 
- When the scancode read from the output buffer is SC_MSB1 (0xE0) or SC_MSB2 (0xE2, as mentioned by professor Pedro Souto), the scancode will have 2 bytes.
- The communication serial line between the keyboard and the keyboard controller sends 1 byte at a time. When the code has 2 bytes, 0xE0 or 0xE2 get sent first
- So I signal that the code has 2 bytes, and the next time it receives an interruption (reads the output buffer), it's gonna be the rest of the code.
- NOT SURE: kbd_print_scancode() probably knows that when scancode_size is 2, the &scancode argument, when printed, is preceded by 0xE0 (don't know if 0xE2 is considered). That's why the &scancode argument is only a uint8_t (doesn't allow the passing of the full 2-byte scancode).
THIS RESULTS IN (WHICH FOR SOME REASON DOESN'T WORK):
  uint8_t scancode_size = 1;
  if (scancode == SC_MSB1 || scancode == SC_MSB2) {
    scancode_size = 2;
    continue;
  }
  else {
    // Makecode MSB is 0, 1st argument needs to be 1 (true)
    // Breakcode MSB is 1, 1st argument needs to be 0 (false)
    if (kbd_print_scancode(!(scancode & BREAKCODE), scancode_size, &scancode)) return 1;
    scancode_size = 1;
  }
*/

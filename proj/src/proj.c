#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include <lcom/timer.h>
#include "devices/timer/aux_timer.h"
#include "devices/timer/i8254.h"

#include "devices/kbc/keyboard.h"
#include "devices/kbc/mouse.h"
#include "devices/kbc/mouse_sm.h"
#include "devices/kbc/i8042.h"

#include "devices/gpu/gpu.h"
#include "devices/gpu/colors_utils.h"
#include "devices/gpu/gpu_macros.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int interrupt_loop() {
  int ipc_status, r;
  message msg;


  uint8_t bit_no;
  if (timer_subscribe_int(&bit_no)) return 1;
  uint32_t timer_int_bit = BIT(bit_no);
  if (kbd_subscribe_int(&bit_no)) return 1;
  uint32_t keyboard_int_bit = BIT(bit_no);
  if (mouse_subscribe_int(&bit_no)) return 1;
  uint32_t mouse_int_bit = BIT(bit_no);
  // if (rtc_subscribe_int(&bit_no)) return 1
  // uint32_t rtc_int_bit = BIT(bit_no);
  // Serial port subscribe
  uint32_t serial_int_bit = BIT(bit_no);

  while (1) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & timer_int_bit) { // subscribed timer interrupt
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & keyboard_int_bit) { // subscribed keyboard interrupt
            kbc_ih();
          }
          if (msg.m_notify.interrupts & mouse_int_bit) { // subscribed mouse interrupt
            mouse_ih();
          }
          if (msg.m_notify.interrupts & serial_int_bit) { // subscribed mouse interrupt

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
  if (kbd_unsubscribe_int()) return 1;
  if (mouse_unsubscribe_int()) return 1;

  return 0;
}

int(proj_main_loop)(int argc, char *argv[]){
  // Start visual manipulation
  if (create_frame_buffer(VBE_MODE_14C)) return 1;
  if (change_video_mode(VBE_MODE_14C)) return 1;

  if (interrupt_loop()) return 1;

  // End visual manipulation
  if (vg_exit()) return 1;

  return 0;
}

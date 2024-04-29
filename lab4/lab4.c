// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "mouse.h"
#include "keyboard.h"

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

uint16_t mouse_bit;
extern uint8_t output;

int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r, byte = 0;
  struct packet res;
  message msg;
  if(mouse_data_reporting(ENABLE_DATA_REPORTING)){
    return 1;
  }
  if(mouse_subscribe_int(&mouse_bit) != 0) {
    return 1;
  }
  while(cnt > 0) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE:				
                if (msg.m_notify.interrupts & mouse_bit) {
                  mouse_ih();
                  if(byte == 0 && (output & BIT(3))){
                    res.lb = output & BIT(0);
                    res.rb = output & BIT(1);
                    res.mb = output & BIT(2);
                    if(output & BIT(4)){
                      res.delta_x |= 0xFF00;
                    }
                    if(output & BIT(5)){
                      res.delta_y |= 0xFF00;
                    }
                    res.x_ov = output & BIT(6);
                    res.y_ov = output & BIT(7);
                    res.bytes[0] = output;
                    byte++;
                  }else if(byte == 1){
                    res.delta_x |= output;
                    res.bytes[1] = output;
                    byte++;
                  }else{
                    res.delta_y |= output;
                    res.bytes[2] = output;
                    byte = 0;
                    mouse_print_packet(&res);
                    cnt--;
                  }
                }
                break;
            default:
                break;
        }
    } else {}
  }
  if(mouse_unsubscribe_int() != 0){
    return 1;
  }
  if(mouse_data_reporting(DISABLE_DATA_REPORTING)){
    return 1;
  }
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042keyboard.h"
#include "i8042mouse.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"

uint8_t packetByte;

int counter_timer = 0;

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

int(mouse_test_packet)(uint32_t cnt) {

  if (mouse_enable_data_reporting()) {
    return 1;
  }

  uint16_t bit_no = 0;
  if (mouse_subscribe(&bit_no)) {
    return 1;
  }

  int ipc_status, r;
  message msg;

  uint8_t mouse_big_packet[3];
  uint8_t counter_byte_packet = 0;

  uint32_t number_of_big_packets = cnt;
  while (number_of_big_packets > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & bit_no) {
            mouse_ih(); // busca o packet byte deste interrupt

            // O primeiro packet da big packet tem de ter o BIT 3 como 1. SENAO TA MAL
            if (counter_byte_packet == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;
            }

            mouse_big_packet[counter_byte_packet] = packetByte;
            counter_byte_packet++;

            if (counter_byte_packet == 3) {
              struct packet big_packet_to_print;

              big_packet_to_print.bytes[0] = mouse_big_packet[0];
              big_packet_to_print.bytes[1] = mouse_big_packet[1];
              big_packet_to_print.bytes[2] = mouse_big_packet[2];

              big_packet_to_print.rb = mouse_big_packet[0] & RIGHT_BUTTON;
              big_packet_to_print.mb = mouse_big_packet[0] & MIDDLE_BUTTON;
              big_packet_to_print.lb = mouse_big_packet[0] & LEFT_BUTTON;

              
              if (mouse_big_packet[0] & DELTAX_MSB){  // bit 4
                big_packet_to_print.delta_x = 0xFF00 | mouse_big_packet[1];
              }else{
                big_packet_to_print.delta_x = mouse_big_packet[1];
              }

              if ((mouse_big_packet[0] & DELTAY_MSB)){ // bit 5
                big_packet_to_print.delta_y = 0xFF00 | mouse_big_packet[2];
              }else{
                big_packet_to_print.delta_y = mouse_big_packet[2];
              }

              big_packet_to_print.x_ov = mouse_big_packet[0] & X_OVER;
              big_packet_to_print.y_ov = mouse_big_packet[0] & Y_OVER;

              mouse_print_packet(&big_packet_to_print);
              number_of_big_packets--;

              counter_byte_packet = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }

  if (mouse_unsubscribe())
    return 1;

  if (disable_stream_data())
    return 1;

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  if (mouse_enable_data_reporting()){
    return 1;
  }

  uint16_t bit_no_mouse = 0;
  uint8_t bit_no_timer = 0;

  if (mouse_subscribe(&bit_no_mouse)) {
    return 1;
  }

  if (timer_subscribe(&bit_no_timer)) {
    return 1;
  }

  int ipc_status, r;
  message msg;

  uint8_t mouse_big_packet[3];
  uint8_t counter_byte_packet = 0;
  int tempo = idle_time;

  while (tempo > 0) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & bit_no_timer) {
            timer_ih();
            if (counter_timer == 60) {
              printf("rime:%d", tempo);
              tempo--;
              counter_timer = 0;
            }
          }

          if (msg.m_notify.interrupts & bit_no_mouse) {

            mouse_ih();

            tempo = idle_time;
            counter_timer = 0;

            if (counter_byte_packet == 0 && !(packetByte & BIT(3))) {
              continue;
            }

            mouse_big_packet[counter_byte_packet] = packetByte;
            counter_byte_packet++;

            if (counter_byte_packet == 3) {

              struct packet big_packet_to_print;

              big_packet_to_print.bytes[0] = mouse_big_packet[0];
              big_packet_to_print.bytes[1] = mouse_big_packet[1];
              big_packet_to_print.bytes[2] = mouse_big_packet[2];

              big_packet_to_print.x_ov = mouse_big_packet[0] & X_OVER; // BIT 6
              big_packet_to_print.y_ov = mouse_big_packet[0] & Y_OVER; // BIT 7

              big_packet_to_print.rb = mouse_big_packet[0] & RIGHT_BUTTON;  // BIT 1
              big_packet_to_print.mb = mouse_big_packet[0] & MIDDLE_BUTTON; // BIT 2
              big_packet_to_print.lb = mouse_big_packet[0] & LEFT_BUTTON;   // BIT 0

              if (mouse_big_packet[0] & DELTAX_MSB){  // bit 4
                big_packet_to_print.delta_x = 0xFF00 | mouse_big_packet[1];
              }else{
                big_packet_to_print.delta_x = mouse_big_packet[1];
              }

              if ((mouse_big_packet[0] & DELTAY_MSB)){ // bit 5
                big_packet_to_print.delta_y = 0xFF00 | mouse_big_packet[2];
              }else{
                big_packet_to_print.delta_y = mouse_big_packet[2];
              }

              mouse_print_packet(&big_packet_to_print);

              counter_byte_packet = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }

  if (timer_unsubscribe()) {
    return 1;
  }

  if (mouse_unsubscribe()) {
    return 1;
  }

  if (disable_stream_data()) {
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  if (enable_stream_data()) {
    return 1;
  }

  uint16_t bit_no = 0;
  if (mouse_subscribe(&bit_no)) {
    return 1;
  }

  int ipc_status, r;
  message msg;

  uint8_t mouse_big_packet[3];
  uint8_t counter_byte_packet = 0;
  uint8_t slope_left = 0;
  uint8_t slope_right = 0;
  uint8_t displacement_left = 0;
  uint8_t displacement_right = 0;
  bool ye = true;
  bool left = false;
  bool right = false;
  bool end = false;
  while (ye) {
    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & bit_no) {
            mouse_ih(); // busca o packet byte deste interrupt

            // O primeiro packet da big packet tem de ter o BIT 3 como 1. SENAO TA MAL
            if (counter_byte_packet == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;
            }

            mouse_big_packet[counter_byte_packet] = packetByte;
            counter_byte_packet++;

            // Parse the 3 packets
            if (counter_byte_packet == 3) {
              struct packet big_packet_to_print;

              big_packet_to_print.bytes[0] = mouse_big_packet[0];
              big_packet_to_print.bytes[1] = mouse_big_packet[1];
              big_packet_to_print.bytes[2] = mouse_big_packet[2];

              big_packet_to_print.rb = mouse_big_packet[0] & RIGHT_BUTTON;
              big_packet_to_print.mb = mouse_big_packet[0] & MIDDLE_BUTTON;
              big_packet_to_print.lb = mouse_big_packet[0] & LEFT_BUTTON;

              if (mouse_big_packet[0] & DELTAX_MSB){  // bit 4
                big_packet_to_print.delta_x = 0xFF00 | mouse_big_packet[1];
              }else{
                big_packet_to_print.delta_x = mouse_big_packet[1];
              }

              if ((mouse_big_packet[0] & DELTAY_MSB)){ // bit 5
                big_packet_to_print.delta_y = 0xFF00 | mouse_big_packet[2];
              }else{
                big_packet_to_print.delta_y = mouse_big_packet[2];
              }

              big_packet_to_print.x_ov = mouse_big_packet[0] & X_OVER;
              big_packet_to_print.y_ov = mouse_big_packet[0] & Y_OVER;

              mouse_print_packet(&big_packet_to_print);
              if ((mouse_big_packet[0] & LEFT_BUTTON) && (!(mouse_big_packet[0] & RIGHT_BUTTON)) && (!(mouse_big_packet[0] & MIDDLE_BUTTON))) {
                if ((-big_packet_to_print.delta_y > tolerance) || (-big_packet_to_print.delta_x > tolerance)) {
                  slope_left = 0;
                  displacement_left = 0;
                  left = false;
                  printf("closed left");
                }
                else {
                  slope_left += big_packet_to_print.delta_y;
                  displacement_left += big_packet_to_print.delta_x;
                  left = true;
                  printf("open left");
                }
                right = false;
                end = false;
              }

              if (left) {
                if (!(mouse_big_packet[0] & LEFT_BUTTON) && (!(mouse_big_packet[0] & RIGHT_BUTTON)) && (!(mouse_big_packet[0] & MIDDLE_BUTTON))) {
                  if ((big_packet_to_print.delta_y < -tolerance) || (big_packet_to_print.delta_y > tolerance) || (-big_packet_to_print.delta_x > tolerance)) {
                    slope_left = 0;
                    displacement_left = 0;
                    left = false;
                    printf("closed left");
                  }
                  else {
                    right = true;
                    printf("open right");
                  }
                }
                else if (((mouse_big_packet[0] & MIDDLE_BUTTON) || ((mouse_big_packet[0] & LEFT_BUTTON) && ((mouse_big_packet[0] & RIGHT_BUTTON))))) {
                left = false;
                right = false;
                end = false;
                }
              }

              if (right){
             
              if ((mouse_big_packet[0] & RIGHT_BUTTON) && (!(mouse_big_packet[0] & LEFT_BUTTON)) && (!(mouse_big_packet[0] & MIDDLE_BUTTON))) {
                if ((big_packet_to_print.delta_y > tolerance) || (-big_packet_to_print.delta_x > tolerance)) {
                  slope_right = 0;
                  displacement_right = 0;
                  left = 0;
                  left = false;
                  right = false;
                  end = false;
                  printf("closed all");
                }
                else {
                  slope_right -= big_packet_to_print.delta_y;
                  displacement_right += big_packet_to_print.delta_x;
                  left = false;
                  end = true;
                  printf("open end");
                }
              }else{
                  slope_left = 0;
                  slope_right = 0;
                  displacement_left = 0;
                  displacement_right = 0;
                  left = 0;
                  left = false;
                  right = false;
                  end = false;
                  printf("closed all");
              }
              }

              if (!(mouse_big_packet[0] & RIGHT_BUTTON) && (!(mouse_big_packet[0] & LEFT_BUTTON)) && (!(mouse_big_packet[0] & MIDDLE_BUTTON)) && end) {
                printf("left: %d", slope_left);
                printf("right: %d", slope_right);
                printf("dis left: %d", displacement_left);
                printf("dis right: %d", displacement_right);
                if ((slope_left > 1) && (slope_right > 1) && (displacement_left >= x_len) && (displacement_right >= x_len)) {
                  ye = false;
                }
              }

              counter_byte_packet = 0;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }

  if (mouse_unsubscribe())
    return 1;
  printf("Aaaaa");
  if (disable_stream_data())
    return 1;

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  /* This year you need not implement this. */
  printf("%s(%u, %u): under construction\n", __func__, period, cnt);
  return 1;
}

// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "aux_timer.h"
#include "mouse.h"
#include "kbc.h"
#include "mouse_sm.h"
#include "i8042.h"


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


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r;
  message msg;
  
  uint8_t pkt_allBytes[3];
  uint8_t pkt_byteCounter = 0;

  // Enable stream mode data reporting
  if (enable_stream_data()) return 1;

  uint8_t bit_no;
  if (mouse_subscribe_int(&bit_no)) return 1;
  uint32_t mouse_int_bit = BIT(bit_no);

  uint32_t numPackets = 0;
  while (numPackets < cnt) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & mouse_int_bit) { // subscribed interrupt
            mouse_ih();
            
            uint8_t packetByte = get_packetByte();

            // Packet's first byte's bit 3 is not 0, (it's not the 1st byte, driver and mouse are desynchronized)
            if (pkt_byteCounter == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;  
            }
            
            pkt_allBytes[pkt_byteCounter++] = packetByte;

            // Parse the 3 packets
            if (pkt_byteCounter == 3) {
              struct packet pkt;

              // Setting up the packet struct
              for (int i = 0; i < 3; i++) {
                pkt.bytes[i] = pkt_allBytes[i];
              }

              pkt.rb = pkt_allBytes[0] & PS2_RB;
              pkt.mb = pkt_allBytes[0] & PS2_MB;
              pkt.lb = pkt_allBytes[0] & PS2_LB;

              // Read notes below
              uint8_t signX = pkt_allBytes[0] & PS2_XDELTA, signY = pkt_allBytes[0] & PS2_YDELTA;
              pkt.delta_x = (signX ? 0xFF00 : 0x0000) | pkt_allBytes[1];
              pkt.delta_y = (signY ? 0xFF00 : 0x0000) | pkt_allBytes[2];

              pkt.x_ov = pkt_allBytes[0] & PS2_XOVER; 
              pkt.y_ov = pkt_allBytes[0] & PS2_YOVER;

              // Successfully parsed and printed a packet
              mouse_print_packet(&pkt);
              numPackets++;

              // Cleaning array and pkt_byteCounter for next packet
              for (int j = 0; j < 3; j++) {
                pkt_allBytes[j] = 0;
              }
              pkt_byteCounter = 0;
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
  
  if (mouse_unsubscribe_int()) return 1;
  
  if (disable_stream_data()) return 1;

  // Reset the mouse to Minix's default configuration (or so I thought) - why doesn't this work?
  // if (kbc_write_cmd(CMD_REG, CMDBYTE_WRITE)) return 1;
  // if (kbc_write_cmd(INBUF_REG, minix_get_dflt_kbc_cmd_byte())) return 1;

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status, r;
  message msg;
  
  uint8_t pkt_allBytes[3];
  uint8_t pkt_byteCounter = 0;

  // Enable stream mode data reporting
  if (enable_stream_data()) return 1;

  uint8_t bit_no;
  if (timer_subscribe_int(&bit_no)) return 1;
  uint32_t timer_int_bit = BIT(bit_no);
  if (mouse_subscribe_int(&bit_no)) return 1;
  uint32_t mouse_int_bit = BIT(bit_no);

  int timePassed = 0;
  while (timePassed < idle_time) {
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
            if (get_timer_intCounter() % sys_hz() == 0) timePassed++;
          }
          if (msg.m_notify.interrupts & mouse_int_bit) { // subscribed mouse  interrupt
            mouse_ih();

            timePassed = 0;
            set_timer_intCounter(0);
            
            uint8_t packetByte = get_packetByte();

            // Packet's first byte's bit 3 is not 0, (it's not the 1st byte, driver and mouse are desynchronized)
            if (pkt_byteCounter == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;  
            }
            
            pkt_allBytes[pkt_byteCounter++] = packetByte;

            // Parse the 3 packets
            if (pkt_byteCounter == 3) {
              struct packet pkt;

              // Setting up the packet struct
              for (int i = 0; i < 3; i++) {
                pkt.bytes[i] = pkt_allBytes[i];
              }

              pkt.rb = pkt_allBytes[0] & PS2_RB;
              pkt.mb = pkt_allBytes[0] & PS2_MB;
              pkt.lb = pkt_allBytes[0] & PS2_LB;

              // Read notes below
              uint8_t signX = pkt_allBytes[0] & PS2_XDELTA, signY = pkt_allBytes[0] & PS2_YDELTA;
              pkt.delta_x = (signX ? 0xFF00 : 0x0000) | pkt_allBytes[1];
              pkt.delta_y = (signY ? 0xFF00 : 0x0000) | pkt_allBytes[2];

              pkt.x_ov = pkt_allBytes[0] & PS2_XOVER; 
              pkt.y_ov = pkt_allBytes[0] & PS2_YOVER;

              // Successfully parsed and printed a packet
              mouse_print_packet(&pkt);

              // Cleaning array and pkt_byteCounter for next packet
              for (int j = 0; j < 3; j++) {
                pkt_allBytes[j] = 0;
              }
              pkt_byteCounter = 0;
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
  if (mouse_unsubscribe_int()) return 1;

  if (disable_stream_data()) return 1;

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status, r;
  message msg;
  
  uint8_t pkt_allBytes[3];
  uint8_t pkt_byteCounter = 0;

  // Enable stream mode data reporting
  if (enable_stream_data()) return 1;

  uint8_t bit_no;
  if (mouse_subscribe_int(&bit_no)) return 1;
  uint32_t mouse_int_bit = BIT(bit_no);

  int done = 0;
  while (!done) {
    // Get a request message.
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & mouse_int_bit) { // subscribed mouse interrupt
            mouse_ih();
            
            uint8_t packetByte = get_packetByte();

            // Packet's first byte's bit 3 is not 0, (it's not the 1st byte, driver and mouse are desynchronized)
            if (pkt_byteCounter == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;  
            }
            
            pkt_allBytes[pkt_byteCounter++] = packetByte;

            // Parse the 3 packets
            if (pkt_byteCounter == 3) {
              struct packet pkt;

              // Setting up the packet struct
              for (int i = 0; i < 3; i++) {
                pkt.bytes[i] = pkt_allBytes[i];
              }

              pkt.rb = pkt_allBytes[0] & PS2_RB;
              pkt.mb = pkt_allBytes[0] & PS2_MB;
              pkt.lb = pkt_allBytes[0] & PS2_LB;

              // Read notes below
              uint8_t signX = pkt_allBytes[0] & PS2_XDELTA, signY = pkt_allBytes[0] & PS2_YDELTA;
              pkt.delta_x = (signX ? 0xFF00 : 0x0000) | pkt_allBytes[1];
              pkt.delta_y = (signY ? 0xFF00 : 0x0000) | pkt_allBytes[2];

              pkt.x_ov = pkt_allBytes[0] & PS2_XOVER; 
              pkt.y_ov = pkt_allBytes[0] & PS2_YOVER;

              // Successfully parsed and printed a packet
              mouse_print_packet(&pkt);
              
              
              done = mouse_eh(&pkt, x_len, tolerance);
              
              
              // Cleaning array and pkt_byteCounter for next packet
              for (int j = 0; j < 3; j++) {
                pkt_allBytes[j] = 0;
              }
              pkt_byteCounter = 0;
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
  
  if (mouse_unsubscribe_int()) return 1;
  
  if (disable_stream_data()) return 1;

  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

/* NOTES: 
<About mouse_test_packet()'s packet setup>
 - If we check documentation of PS2_XDELTA and PS_YDELTA, we realise they dictate the signedness of the 9-bit 2's complement integers.
 - These integers are formed by:
  - X delta: bit 4 of packet 1 and packet 2;
  - Y delta: bit 5 of packet 1 and packet 3;
 - If we check the documentation of struct packet's delta_x and delta_y fields, we realise that they are signed 16-bit integers, so, depending on the value of bit 4 and 5, the 8 MSBs of both deltas will have to be either 0xFF or 0x00 (concept of sign extension). 
*/

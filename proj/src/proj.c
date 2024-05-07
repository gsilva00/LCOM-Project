#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "devices/timer/aux_timer.h"
#include "devices/timer/i8254.h"
#include <lcom/timer.h>

#include "devices/kbc/i8042.h"
#include "devices/kbc/keyboard.h"
#include "devices/kbc/mouse.h"
#include "devices/kbc/mouse_sm.h"

#include "devices/gpu/colors_utils.h"
#include "devices/gpu/gpu.h"
#include "devices/gpu/gpu_macros.h"

#include "objects/ball.h"
#include "objects/bola.xpm"
#include "objects/object_controllers/ball_controller.h"
#include "objects/object_controllers/player_controller.h"

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
  return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {
  // ======= Start graphics =======
  if (create_frame_buffer(VBE_MODE_115)) {
    printf("Error while creating frame buffer!\n");
    return 1;
  }
  if (change_video_mode(VBE_MODE_115)) {
    printf("Error while changing video mode!\n");
    return 1;
  }

  // ======= Interrupt loop =======
  int ipc_status, r;
  message msg;

  uint8_t bit_no;
  if (timer_subscribe_int(&bit_no)) {
    printf("Error while subscribing timer ints!\n");
    return 1;
  }
  uint32_t timer_int_bit = BIT(bit_no);
  if (kbd_subscribe_int(&bit_no)) {
    printf("Error while subscribing kbd ints!\n");
    return 1;
  }
  uint32_t keyboard_int_bit = BIT(bit_no);
  if (mouse_subscribe_int(&bit_no)) {
    printf("Error while subscribing mouse ints!\n");
    return 1;
  }
  uint32_t mouse_int_bit = BIT(bit_no);
  // if (rtc_subscribe_int(&bit_no)) return 1
  // uint32_t rtc_int_bit = BIT(bit_no);
  // Serial port subscribe
  uint32_t serial_int_bit = BIT(bit_no);
  bool done = false;

  uint32_t background_color = 0x550055;
  if (draw_background(background_color)) {
    return 1; // epa nao deu pra desenhar background
  }

  xpm_map_t bola_map = (xpm_map_t) bola_xpm;

  printf("bola %s\n", bola_map[0]);
  // printf("Aa");

  // xpm_image_t img;
  ball *bola = create_ball(bola_map, 50, 200, 10, 50, 0);

  // Draw the XPM image
  draw_xpm(bola->x, bola->y, bola->img);

  BallState ball_state = STATE_NONE;
  BallState ball_state_temporary = STATE_NONE;

  player *player;
  PlayerState player_state = STATE_NONE;
  PlayerState player_state_temporary = STATE_NONE;

  // int counter = 0;
  while (!done) {
    // Get a request message.
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & timer_int_bit) {
            timer_int_handler();

            move_ball(bola,&ball_state,&ball_state_temporary);
            move_player(player, &player_state, &player_state_temporary);
          }
          if (msg.m_notify.interrupts & keyboard_int_bit) { // subscribed keyboard interrupt
            kbc_ih();
            uint8_t scancode = get_scancode();
            bool make = 1;
            if (scancode & BREAKCODE) {
              make = 0;
            }

            if (kbd_print_scancode(make, 1, &scancode)) {
              return 1;
            }

            if (scancode == 0x19) { // 0x19 is the scancode for the "P" key
              if (ball_state != STATE_NONE) {
                ball_state = STATE_JUMP_END;
                ball_state_temporary = STATE_MOVE_RIGHT_START;
              }else{
                ball_state = STATE_MOVE_RIGHT_START;
              }
            }

            if (scancode == 0x1e) { // 0x19 is the scancode for the "P" key
             if (ball_state != STATE_NONE) {
              ball_state = STATE_JUMP_END;
              ball_state_temporary = STATE_MOVE_LEFT_START;
            }else{
              ball_state = STATE_MOVE_LEFT_START;
            }
            }

            if (0xcd == scancode) {
              if (ball_state != STATE_NONE) {
                ball_state = STATE_JUMP_END;
                ball_state_temporary = STATE_START_JUMP_RIGHT;
              }
              else {
                ball_state = STATE_START_JUMP_RIGHT;
              }
            }

            if (0xcb == scancode) {
              if (ball_state != STATE_NONE) {
                ball_state = STATE_JUMP_END;
                ball_state_temporary = STATE_START_JUMP_LEFT;
              }
              else {
                ball_state = STATE_START_JUMP_LEFT;
              }
            }

            if (BREAKCODE_ESC == scancode) {
              done = true;
            }
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

  destroy_ball(bola);

  if (timer_unsubscribe_int()) {
    printf("Error while unsubscribing timer ints!\n");
    return 1;
  }
  if (kbd_unsubscribe_int()) {
    printf("Error while unsubscribing kbd ints!\n");
    return 1;
  }
  if (mouse_unsubscribe_int()) {
    printf("Error while unsubscribing mouse ints!\n");
    return 1;
  }

  if (limpa_buffer()) {
    return 1;
  }

  // ======= End graphics =======
  if (vg_exit()) {
    printf("Error while returning to text mode!\n");
    return 1;
  }

  return 0;
}

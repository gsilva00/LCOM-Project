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
#include "objects/xpm/bola.xpm"
#include "objects/xpm/muro.xpm"
#include "objects/xpm/baliza-tras-right.xpm"
#include "objects/xpm/goal_back.xpm"
#include "objects/goal.h"
#include "objects/xpm/personagem_parado.xpm"
#include "objects/xpm/menuuu.xpm"
#include "objects/xpm/pausa_menu_90_sem_background.xpm"
#include "objects/xpm/play.xpm"
#include "objects/xpm/play_sem.xpm"
#include "objects/xpm/close.xpm"
#include "objects/xpm/close_sem.xpm"
#include "objects/wall.h"
#include "objects/object_controllers/ball_controller.h"
#include "objects/object_controllers/player_controller.h"
#include "menu.h"
#include "objects/button.h"

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

typedef enum {
  STATE_GAME_START,
  STATE_GAME_START_TO_PLAY,
  STATE_GAME_PLAY,
  STATE_GAME_PLAY_TO_START,
  STATE_GAME_PLAY_TO_PAUSE,
  STATE_GAME_PAUSE,
  STATE_GAME_PAUSE_TO_START,
  STATE_GAME_PAUSE_TO_PLAY,
  STATE_GAME_OVER
} GameState;

int keyboard_active = 1;
int mouse_active = 1;

int draw_game(wall *muro_, goal *goal_, goal *goal_right_, ball *bola, player *player, bool done){

  if (draw_back(0, 600 - 314, muro_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(0, 314, goal_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(720, 314, goal_right_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  draw_frame_start();

  // Draw the XPM image
  draw_xpm(bola->x, bola->y, bola->img);

  draw_xpm(player->x, player->y, player->img);

  if(done) draw_frame_end();
  return 0;
}

void draw_main_menu(xpm_image_t menu_img, button *single, button *multi, button *end){
  draw_xpm(0, 0, menu_img);
  if(single != NULL) draw_xpm(single->x, single->y, single->img);
  if(multi != NULL) draw_xpm(multi->x, multi->y, multi->img);
  if(end != NULL) draw_xpm(end->x, end->y, end->img);
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
  bool done = false;
  uint8_t bit_no;
  BallState ball_state = STATE_NONE;
  BallState ball_state_temporary = STATE_NONE;
  PlayerStateJump player_state_jump = STATE_PLAYER_JUMP_NONE;
  PlayerStateJump player_state_jump_temporary = STATE_PLAYER_JUMP_NONE;
  PlayerStateMove player_state_move = STATE_PLAYER_MOVE_NONE;
  PlayerStateMove player_state_move_temporary = STATE_PLAYER_MOVE_NONE;
  GameState game_state = STATE_GAME_START;
  MenuState menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
  MenuPauseState menu_pause_state = STATE_MENU_HOVER_RESUME;
  ball *bola;
  player *player;
  button *single;
  button *multi;
  button *end;
  button *resume;
  button *go_back;
  xpm_map_t muro = (xpm_map_t) muro_xpm;
  wall *muro_ = create_wall(muro);
  xpm_map_t baliza = (xpm_map_t) goal_back_xpm;
  xpm_map_t baliza_right = (xpm_map_t) baliza_tras_right_xpm;
  goal *goal_ = create_goal(baliza);
  goal *goal_right_ = create_goal(baliza_right);
  xpm_map_t bola_map = (xpm_map_t) bola_xpm;
  xpm_map_t player_map = (xpm_map_t) personagem_parado_xpm;
  xpm_map_t start_selected_map = (xpm_map_t) play_xpm;
  xpm_map_t start_not_selected_map = (xpm_map_t) play_sem_xpm;
  xpm_map_t end_selected_map = (xpm_map_t) close_xpm;
  xpm_map_t end_not_selected_map = (xpm_map_t) close_sem_xpm;
  xpm_map_t menu_xpm = (xpm_map_t) menuuu_xpm;
  xpm_map_t pause_menu_xpm = (xpm_map_t) pausa_menu_90_sem_background_xpm;
  xpm_image_t menu_img;
  xpm_image_t pause_menu_img;
  bola = create_ball(bola_map, 20, 200, 10, 50, 0);
  player = create_player(player_map, 400, 400, 10, 50, 0);
  single = create_button(start_selected_map, 500, 155, true);
  multi = create_button(start_not_selected_map, 500, 219, false);
  end = create_button(end_not_selected_map, 500, 284, false);
  resume = create_button(start_selected_map, 500, 155, true);
  go_back = create_button(end_not_selected_map, 500, 219, false);

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


  if(game_state == STATE_GAME_START){
    xpm_load(menu_xpm, XPM_8_8_8, &menu_img);
    draw_frame_start();
    draw_main_menu(menu_img, single, multi, end);
    draw_frame_end();
  }

  if(game_state == STATE_GAME_PLAY){
    if(draw_game(muro_, goal_, goal_right_, bola, player, true)){
      return 1;
    }
  }

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

            if (get_timer_intCounter() % 2 == 0) {
              if(game_state == STATE_GAME_PLAY){
                if (ball_state != STATE_NONE){
                  draw_frame_start();
                }
                if (player_state_move !=  STATE_PLAYER_MOVE_NONE){
                  draw_frame_start();
                }

                draw_xpm(bola->x, bola->y, bola->img);
                draw_xpm(player->x, player->y, player->img);
              }
              else if(game_state == STATE_GAME_START){
                if(menu_state == STATE_MENU_HOVER_SINGLEPLAYER && single->selected == false){
                  set_selected(single, true);
                  set_selected(multi, false);
                  set_selected(end, false);
                  set_image(single, start_selected_map);
                  set_image(multi, start_not_selected_map);
                  set_image(end, end_not_selected_map);
                  draw_frame_start();
                  draw_main_menu(menu_img, single, multi, end);
                  draw_frame_end();
                }
                if(menu_state == STATE_MENU_HOVER_MULTIPLAYER && multi->selected == false){
                  set_selected(single, false);
                  set_selected(multi, true);
                  set_selected(end, false);
                  set_image(single, start_not_selected_map);
                  set_image(multi, start_selected_map);
                  set_image(end, end_not_selected_map);
                  draw_frame_start();
                  draw_main_menu(menu_img, single, multi, end);
                  draw_frame_end();
                }
                else if(menu_state == STATE_MENU_HOVER_EXIT && end->selected == false){
                  set_selected(end, true);
                  set_selected(multi, false);
                  set_selected(single, false);
                  set_image(end, end_selected_map);
                  set_image(multi, start_not_selected_map);
                  set_image(single, start_not_selected_map);
                  draw_frame_start();
                  draw_main_menu(menu_img, single, multi, end);
                  draw_frame_end();
                }
              }
              else if(game_state == STATE_GAME_START_TO_PLAY){
                set_selected(single, true);
                set_selected(multi, false);
                set_selected(end, false);
                draw_game(muro_, goal_, goal_right_, bola, player, true);
                game_state = STATE_GAME_PLAY;
              }
              else if(game_state == STATE_GAME_PAUSE_TO_START){
                set_selected(resume, true);
                set_selected(go_back, false);
                draw_frame_start();
                draw_main_menu(menu_img, single, multi, end);
                draw_frame_end();
                game_state = STATE_GAME_START;
              }
              else if(game_state == STATE_GAME_PLAY_TO_PAUSE){
                xpm_load(pause_menu_xpm, XPM_8_8_8, &pause_menu_img);
                draw_main_menu(pause_menu_img, resume, go_back, NULL);
                draw_frame_end();
                game_state = STATE_GAME_PAUSE;
              }
              else if(game_state == STATE_GAME_PAUSE_TO_PLAY){
                set_selected(resume, true);
                set_selected(go_back, false);
                draw_game(muro_, goal_, goal_right_, bola, player, true);
                game_state = STATE_GAME_PLAY;
              }
              else if(game_state == STATE_GAME_PAUSE){
                if(menu_pause_state == STATE_MENU_HOVER_RESUME && resume->selected == false){
                  set_selected(resume, true);
                  set_selected(go_back, false);
                  set_image(resume, start_selected_map);
                  set_image(go_back, end_not_selected_map);
                  draw_game(muro_, goal_, goal_right_, bola, player, false);
                  draw_main_menu(pause_menu_img, resume, go_back, NULL);
                  draw_frame_end();
                }
                if(menu_pause_state == STATE_MENU_HOVER_GO_BACK && go_back->selected == false){
                  set_selected(resume, false);
                  set_selected(go_back, true);
                  set_image(resume, start_not_selected_map);
                  set_image(go_back, end_selected_map);
                  draw_game(muro_, goal_, goal_right_, bola, player, false);
                  draw_main_menu(pause_menu_img, resume, go_back, NULL);
                  draw_frame_end();
                }
              }
            }
            
            if(game_state == STATE_GAME_PLAY){
              move_ball(bola,&ball_state,&ball_state_temporary);
              move_player(player, &player_state_move, &player_state_move_temporary, &player_state_jump, &player_state_jump_temporary);
              draw_frame_end();
            }
          }
          if ((msg.m_notify.interrupts & keyboard_int_bit) && keyboard_active) { // subscribed keyboard interrupt
            kbc_ih();
            uint8_t scancode = get_scancode();
            bool make = 1;
            if (scancode & BREAKCODE) {
              make = 0;
            }

            if (kbd_print_scancode(make, 1, &scancode)) {
              return 1;
            }

            if (scancode == MAKECODE_P) { // 0x19 is the makecode for the "P" key
              if(game_state == STATE_GAME_PLAY){
                if (ball_state != STATE_NONE) {
                  ball_state = STATE_JUMP_END;
                  ball_state_temporary = STATE_MOVE_RIGHT_START;
                }else{
                  ball_state = STATE_MOVE_RIGHT_START;
                }
              }
            }

            if (scancode == MAKECODE_A) { // 0x1e is the makecode for the "A" key
              if(game_state == STATE_GAME_PLAY){
                if (ball_state != STATE_NONE) {
                  ball_state = STATE_JUMP_END;
                  ball_state_temporary = STATE_MOVE_LEFT_START;
                }else{
                  ball_state = STATE_MOVE_LEFT_START;
                }
              }
            }

            if (MAKECODE_RIGHT == scancode) { //0x4d is the makecode for the right key
              if(game_state == STATE_GAME_PLAY){
                if (ball_state != STATE_NONE) {
                  ball_state = STATE_JUMP_END;
                  ball_state_temporary = STATE_START_JUMP_RIGHT;
                }
                else {
                  ball_state = STATE_START_JUMP_RIGHT;
                }
                if (player_state_move != STATE_PLAYER_MOVE_NONE) {
                  player_state_move = STATE_PLAYER_MOVE_END;
                  player_state_move_temporary = STATE_PLAYER_MOVE_RIGHT_START;
                }else{
                  player_state_move = STATE_PLAYER_MOVE_RIGHT_START;
                }
              }
            }

            if (MAKECODE_LEFT == scancode) { //0x4b is the makecode for the left key
              if(game_state == STATE_GAME_PLAY){
                if (ball_state != STATE_NONE) {
                  ball_state = STATE_JUMP_END;
                  ball_state_temporary = STATE_START_JUMP_LEFT;
                }
                else {
                  ball_state = STATE_START_JUMP_LEFT;
                }
                if (player_state_move != STATE_PLAYER_MOVE_NONE) {
                  player_state_move = STATE_PLAYER_MOVE_END;
                  player_state_move_temporary = STATE_PLAYER_MOVE_LEFT_START;
                }else{
                  player_state_move = STATE_PLAYER_MOVE_LEFT_START;
                }
              }
            }

            if(BREAKCODE_LEFT == scancode){ //0xcd is the breakcode for the right key
              if(game_state == STATE_GAME_PLAY){
                player_state_move = STATE_AFTER_PLAYER_MOVE_RIGHT;
              }
            }

            if(BREAKCODE_RIGHT == scancode){ //0xcb is the breakcode for the left key
              if(game_state == STATE_GAME_PLAY){
                player_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
              }
            }

            if(MAKECODE_UP == scancode){ //0x48 is the makecode for the up key
              if(game_state == STATE_GAME_PLAY){
                if (player_state_jump != STATE_PLAYER_JUMP_NONE) {
                  player_state_jump = STATE_PLAYER_JUMP_END;
                  player_state_jump_temporary = STATE_START_PLAYER_JUMP;
                }
                else {
                  player_state_jump = STATE_START_PLAYER_JUMP;
                }
              }else if(game_state == STATE_GAME_START){
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER){
                  menu_state = STATE_MENU_HOVER_EXIT;
                }else if(menu_state == STATE_MENU_HOVER_MULTIPLAYER){
                  menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
                }else if(menu_state == STATE_MENU_HOVER_EXIT){
                  menu_state = STATE_MENU_HOVER_MULTIPLAYER;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }
            }

            if(MAKECODE_DOWN == scancode){ //0x50 is the makecode for the down key
              if(game_state == STATE_GAME_START){
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER){
                  menu_state = STATE_MENU_HOVER_MULTIPLAYER;
                }else if(menu_state == STATE_MENU_HOVER_MULTIPLAYER){
                  menu_state = STATE_MENU_HOVER_EXIT;
                }else if(menu_state == STATE_MENU_HOVER_EXIT){
                  menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }
            }

            if(MAKECODE_ENTER == scancode){ //0x50 is the makecode for the enter key
              if(game_state == STATE_GAME_START){
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER){
                  game_state = STATE_GAME_START_TO_PLAY;
                }else if (menu_state == STATE_MENU_HOVER_MULTIPLAYER){
                  game_state = STATE_GAME_START_TO_PLAY;
                }else if(menu_state == STATE_MENU_HOVER_EXIT){
                  game_state = STATE_GAME_OVER;
                  done = true;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  game_state = STATE_GAME_PAUSE_TO_PLAY;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  game_state = STATE_GAME_PAUSE_TO_START;
                }
              }
            }

            if (BREAKCODE_ESC == scancode) {
              if(game_state == STATE_GAME_PLAY){
                game_state = STATE_GAME_PLAY_TO_PAUSE;
              }
            }
          }
          if ((msg.m_notify.interrupts & mouse_int_bit) && mouse_active) { // subscribed mouse interrupt
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
  destroy_player(player);

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

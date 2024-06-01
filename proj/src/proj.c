#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "devices/timer/aux_timer.h"
#include "devices/timer/i8254.h"
#include <lcom/timer.h>

#include "devices/kbc/i8042.h"
#include "devices/kbc/keyboard.h"
#include "devices/kbc/mouse.h"

#include "devices/gpu/gpu.h"
#include "devices/gpu/gpu_macros.h"

#include "devices/rtc/rtc.h"

#include "devices/serial_port/uart.h"

#include "menu.h"

#include "objects/ball.h"
#include "objects/button.h"
#include "objects/cursor.h"
#include "objects/goal.h"
#include "objects/scoreboard.h"
#include "objects/timeboard.h"
#include "objects/wall.h"

#include "objects/object_controllers/ball_controller.h"
#include "objects/object_controllers/player_controller.h"

#include "images/objects/baliza-tras-right.xpm"
#include "images/objects/bola.xpm"
#include "images/objects/cursor.xpm"
#include "images/objects/goal_back.xpm"
#include "images/objects/goal_front.xpm"
#include "images/objects/goal_front_right.xpm"
#include "images/objects/goal_card.xpm"
#include "images/objects/scoreboard_90.xpm"
#include "images/objects/timeboard_90.xpm"
#include "images/backgrounds/muro.xpm"
#include "images/buttons/play.xpm"
#include "images/buttons/play_sem.xpm"
#include "images/buttons/close.xpm"
#include "images/buttons/close_sem.xpm"
#include "images/menus/draw_menu.xpm"
#include "images/menus/menu.xpm"
#include "images/menus/pausa_menu_90_sem_background.xpm"
#include "images/menus/player1win_menu.xpm"
#include "images/menus/player2win_menu.xpm"
#include "images/menus/timereached_menu.xpm"


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
  STATE_GAME_START_TO_PLAY_MULTIPLAYER,
  STATE_GAME_PLAY_MULTIPLAYER,
  STATE_GAME_START_TO_PLAY,
  STATE_GAME_PLAY,
  STATE_GAME_PLAY_TO_START,
  STATE_GAME_PLAY_TO_PAUSE,
  STATE_GAME_PAUSE,
  STATE_GAME_PAUSE_TO_START,
  STATE_GAME_PAUSE_TO_PLAY,
  STATE_GAME_END,
  STATE_GAME_TIME_REACHED,
  STATE_GAME_GOAL,
  STATE_GAME_OVER
} GameState;

int keyboard_active = 1;
int mouse_active = 1;

uint8_t mouse_big_packet[3];
uint8_t counter_byte_packet = 0;

void enter_new_state(GameState *game_state, MenuState *menu_state, MenuPauseState *menu_pause_state, bool *done) {
  if (*game_state == STATE_GAME_START) {
    if (*menu_state == STATE_MENU_HOVER_SINGLEPLAYER) {
      *game_state = STATE_GAME_START_TO_PLAY;
    }
    else if (*menu_state == STATE_MENU_HOVER_MULTIPLAYER) {
      *game_state = STATE_GAME_START_TO_PLAY_MULTIPLAYER;
    }
    else if (*menu_state == STATE_MENU_HOVER_EXIT) {
      *game_state = STATE_GAME_OVER;
      *done = true;
    }
  }
  else if (*game_state == STATE_GAME_PAUSE) {
    if (*menu_pause_state == STATE_MENU_HOVER_RESUME) {
      *game_state = STATE_GAME_PAUSE_TO_PLAY;
    }
    else if (*menu_pause_state == STATE_MENU_HOVER_GO_BACK) {
      *game_state = STATE_GAME_PAUSE_TO_START;
    }
  }
  else if (*game_state == STATE_GAME_END || *game_state == STATE_GAME_TIME_REACHED) {
    *game_state = STATE_GAME_PAUSE_TO_START;
  }
}

bool is_cursor_touching(cursor *cursor, button *button) {
  uint16_t cursor_end = cursor->x + cursor->img.width;
  uint16_t cursor_bottom = cursor->y + cursor->img.height;
  uint16_t button_end = button->x + button->img.width;
  uint16_t button_bottom = button->y + button->img.height;
  return !((cursor_bottom < button->y || button_bottom < cursor->y) || (cursor_end < button->x || button_end < cursor->x));
}

bool check_cursor_position(cursor *cursor, button *single, button *multi, button *end, GameState game_state, MenuState *menu_state, MenuPauseState *menu_pause_state) {

  bool is_touching_single = (single != NULL) ? is_cursor_touching(cursor, single) : false;
  bool is_touching_multi = (multi != NULL) ? is_cursor_touching(cursor, multi) : false;
  bool is_touching_end = (end != NULL) ? is_cursor_touching(cursor, end) : false;

  if (is_touching_single) {
    if (game_state == STATE_GAME_START && menu_state != NULL) {
      *menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
    }
    else if (game_state == STATE_GAME_PAUSE && menu_pause_state != NULL) {
      *menu_pause_state = STATE_MENU_HOVER_RESUME;
    }
  }
  if (is_touching_multi) {
    if (game_state == STATE_GAME_START && menu_state != NULL) {
      *menu_state = STATE_MENU_HOVER_MULTIPLAYER;
    }
    else if (game_state == STATE_GAME_PAUSE && menu_pause_state != NULL) {
      *menu_pause_state = STATE_MENU_HOVER_GO_BACK;
    }
  }
  if (is_touching_end) {
    if (game_state == STATE_GAME_START && menu_state != NULL) {
      *menu_state = STATE_MENU_HOVER_EXIT;
    }
  }
  return is_touching_single || is_touching_multi || is_touching_end;
}

void reset_states(BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump, bool multiplayer) {
  if (*ball_state != STATE_NONE) {
    *ball_state = STATE_JUMP_END;
  }
  else {
    *ball_state = STATE_NONE;
  }
  if (*player1_state_move != STATE_PLAYER_MOVE_NONE) {
    *player1_state_move = STATE_PLAYER_MOVE_RIGHT_END;
    *player1_state_jump = STATE_PLAYER_JUMP_END;
  }
  else {
    *player1_state_move = STATE_PLAYER_MOVE_NONE;
    *player1_state_jump = STATE_PLAYER_JUMP_NONE;
  }

  if (multiplayer && player2_state_move != NULL && player2_state_jump != NULL) {
    if (*player2_state_move != STATE_PLAYER_MOVE_NONE) {
      *player2_state_move = STATE_PLAYER_MOVE_RIGHT_END;
      *player2_state_jump = STATE_PLAYER_JUMP_END;
    }
    else {
      *player2_state_move = STATE_PLAYER_MOVE_NONE;
      *player2_state_jump = STATE_PLAYER_JUMP_NONE;
    }
  }
}

int reset_game_multiplayer(scoreboard *scoreboard, timeboard *tb, ball *bola, player *player1, player *player2, BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump) {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  tb->time = 300;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  move_player2_to_center(player2);
  reset_states(ball_state, player1_state_move, player2_state_move, player1_state_jump, player2_state_jump, player2 != NULL);
  return 0;
}

int reset_game_singleplayer(scoreboard *scoreboard, timeboard *tb, ball *bola, player *player1, BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateJump *player1_state_jump) {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  tb->time = 600;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  reset_states(ball_state, player1_state_move, NULL, player1_state_jump, NULL, false);
  return 0;
}

int draw_game(wall *muro_, goal *goal_, goal *goal_right_, scoreboard *scoreboard, timeboard *tb, ball *bola, player *player1, player *player2, xpm_image_t goal_img, xpm_image_t goal_right_img, bool done) {

  if (draw_back(0, 600 - 314, muro_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(goal_->x, goal_->y, goal_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(goal_right_->x, goal_right_->y, goal_right_->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(scoreboard->x, scoreboard->y, scoreboard->img)) {
    return 1;
  }

  if (draw_back_scoreboard(scoreboard->x + scoreboard->img.width / 5, scoreboard->y + scoreboard->img.height / 3, scoreboard->points1)) {
    printf("Error while drawing scoreboard\n");
    return 1;
  }

  if (draw_back_scoreboard(scoreboard->x + scoreboard->img.width / 2 + scoreboard->img.width / 5, scoreboard->y + scoreboard->img.height / 3, scoreboard->points2)) {
    return 1;
  }

  if (draw_back(tb->x, tb->y, tb->img)) {
    return 1;
  }

  int minutes = tb->time / 60;
  int seconds = tb->time % 60;

  // To draw the minutes in the timeboard
  draw_back_scoreboard(tb->x + 15, tb->y + tb->img.height / 3, minutes / 10);
  draw_back_scoreboard(tb->x + 35, tb->y + tb->img.height / 3, minutes % 10);

  // To draw the seconds in the timeboard
  draw_back_scoreboard(tb->x + 80, tb->y + tb->img.height / 3, seconds / 10);
  draw_back_scoreboard(tb->x + 100, tb->y + tb->img.height / 3, seconds % 10);

  draw_frame_start();

  // To draw characters from the game
  draw_xpm(bola->x, bola->y, bola->img, false);

  draw_xpm(player1->x, player1->y, player1->img, true);
  if (player2 != NULL) {
    draw_xpm(player2->x, player2->y, player2->img, true);
  }

  draw_xpm(goal_->x - 8, goal_->y - 18, goal_img, true);
  draw_xpm(goal_right_->x, goal_right_->y - 18, goal_right_img, true);

  if (done)
    draw_frame_end();
  return 0;
}

void draw_main_menu(xpm_image_t menu_img, button *single, button *multi, button *end, cursor *cursor_) {
  draw_xpm(0, 0, menu_img, true);
  if (single != NULL)
    draw_xpm(single->x, single->y, single->img, true);
  if (multi != NULL)
    draw_xpm(multi->x, multi->y, multi->img, true);
  if (end != NULL)
    draw_xpm(end->x, end->y, end->img, true);
  if (cursor_ != NULL)
    draw_xpm(cursor_->x, cursor_->y, cursor_->img, true);
}

void restart_game(ball *bl, BallState *ball_state, player *player1, player *player2, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump) {
  move_ball_to_center(bl);
  move_player1_to_center(player1);
  if (player2 != NULL) {
    move_player2_to_center(player2);
  }
  reset_states(ball_state, player1_state_move, player2_state_move, player1_state_jump, player2_state_jump, player2 != NULL);
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
  bool done = false, touching1 = false, touching2 = false;
  uint8_t bit_no;
  BallState ball_state = STATE_NONE;
  BallState ball_state_temporary = STATE_NONE;
  PlayerStateJump player1_state_jump = STATE_PLAYER_JUMP_NONE;
  PlayerStateJump player1_state_jump_temporary = STATE_PLAYER_JUMP_NONE;
  PlayerStateMove player1_state_move = STATE_PLAYER_MOVE_NONE;
  PlayerStateMove player1_state_move_temporary = STATE_PLAYER_MOVE_NONE;
  PlayerStateJump player2_state_jump = STATE_PLAYER_JUMP_NONE;
  PlayerStateJump player2_state_jump_temporary = STATE_PLAYER_JUMP_NONE;
  PlayerStateMove player2_state_move = STATE_PLAYER_MOVE_NONE;
  PlayerStateMove player2_state_move_temporary = STATE_PLAYER_MOVE_NONE;
  PlayerStateKick player1_state_kick = STATE_PLAYER_KICK_NONE;
  PlayerStateKick player1_state_kick_temporary = STATE_PLAYER_KICK_NONE;
  PlayerStateKick player2_state_kick = STATE_PLAYER_KICK_NONE;
  PlayerStateKick player2_state_kick_temporary = STATE_PLAYER_KICK_NONE;
  GameState game_state = STATE_GAME_START;
  MenuState menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
  MenuPauseState menu_pause_state = STATE_MENU_HOVER_RESUME;
  cursor *cursor;
  ball *bola;
  player *player1;
  player *player2;
  button *single;
  button *multi;
  button *end;
  button *resume;
  button *go_back;
  xpm_map_t muro = (xpm_map_t) muro_xpm;
  wall *muro_ = create_wall(muro);
  xpm_map_t baliza = (xpm_map_t) goal_back_xpm;
  xpm_map_t baliza_right = (xpm_map_t) baliza_tras_right_xpm;
  xpm_map_t baliza_front = (xpm_map_t) goal_front_xpm;
  xpm_map_t baliza_right_front = (xpm_map_t) goal_front_right_xpm;
  goal *goal_ = create_goal(baliza, 0);
  goal *goal_right_ = create_goal(baliza_right, 1);
  xpm_image_t goal_img;
  xpm_image_t goal_right_img;
  xpm_load(baliza_front, XPM_8_8_8, &goal_img);
  xpm_load(baliza_right_front, XPM_8_8_8, &goal_right_img);
  xpm_map_t scoreboard_map = (xpm_map_t) scoreboard_90_xpm;
  scoreboard *scoreboard = create_scoreboard(scoreboard_map);
  xpm_map_t tb_map = (xpm_map_t) timeboard_90_xpm;
  timeboard *tb = create_timeboard(tb_map);
  xpm_map_t cursor_map = (xpm_map_t) cursor_xpm;
  xpm_map_t bola_map = (xpm_map_t) bola_xpm;
  xpm_map_t start_selected_map = (xpm_map_t) play_xpm;
  xpm_map_t start_not_selected_map = (xpm_map_t) play_sem_xpm;
  xpm_map_t end_selected_map = (xpm_map_t) close_xpm;
  xpm_map_t end_not_selected_map = (xpm_map_t) close_sem_xpm;
  xpm_map_t menu_xpm_ = (xpm_map_t) menu_xpm;
  xpm_map_t pause_menu_xpm = (xpm_map_t) pausa_menu_90_sem_background_xpm;
  xpm_map_t player1_map0 = (xpm_map_t) personagem_parado_xpm;
  xpm_map_t player2_map0 = (xpm_map_t) personagem_2_parado_xpm;
  xpm_map_t player1_win_menu_xpm = (xpm_map_t) player1win_menu_xpm;
  xpm_map_t player2_win_menu_xpm = (xpm_map_t) player2win_menu_xpm;
  xpm_map_t windraw_menu_xpm = (xpm_map_t) draw_menu_xpm;
  xpm_map_t time_reached_menu_xpm = (xpm_map_t) timereached_menu_xpm;
  xpm_map_t goal_card_menu_xpm = (xpm_map_t) goal_card_xpm;
  xpm_image_t menu_img;
  xpm_image_t pause_menu_img;
  xpm_image_t player1win_menu_img;
  xpm_image_t player2win_menu_img;
  xpm_image_t draw_menu_img;
  xpm_image_t time_reached_menu_img;
  xpm_image_t goal_card_menu_img;
  cursor = create_cursor(cursor_map);
  bola = create_ball(bola_map, 400, 490, 32, 32, 10, 50, 0);
  player1 = create_player(player1_map0, 200, 450, 62, 78, 10, 50, 0);
  player2 = create_player(player2_map0, 600, 450, 62, 78, 10, 50, 1);
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
  if (rtc_subscribe_int(&bit_no)) {
    printf("Error while subscribing rtc ints!\n"); 
    return 1;
  }
  uint32_t rtc_int_bit = BIT(bit_no);
  if (uart_subscribe_int(&bit_no)) {
    printf("Error while subscribing uart ints!\n"); 
    return 1;
  }
  uint32_t uart_int_bit = BIT(bit_no);


  if (game_state == STATE_GAME_START) {
    xpm_load(menu_xpm_, XPM_8_8_8, &menu_img);
    draw_frame_start();
    draw_main_menu(menu_img, single, multi, end, cursor);
    draw_frame_end();
  }

  if (game_state == STATE_GAME_PLAY) {
    if (draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true)) {
      return 1;
    }
  }
  if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
    if (draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true)) {
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
              if (game_state == STATE_GAME_START) {
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER && single->selected == false) {
                  set_selected(single, true);
                  set_selected(multi, false);
                  set_selected(end, false);
                  set_image(single, start_selected_map);
                  set_image(multi, start_not_selected_map);
                  set_image(end, end_not_selected_map);
                }
                if (menu_state == STATE_MENU_HOVER_MULTIPLAYER && multi->selected == false) {
                  set_selected(single, false);
                  set_selected(multi, true);
                  set_selected(end, false);
                  set_image(single, start_not_selected_map);
                  set_image(multi, start_selected_map);
                  set_image(end, end_not_selected_map);
                }
                else if (menu_state == STATE_MENU_HOVER_EXIT && end->selected == false) {
                  set_selected(end, true);
                  set_selected(multi, false);
                  set_selected(single, false);
                  set_image(end, end_selected_map);
                  set_image(multi, start_not_selected_map);
                  set_image(single, start_not_selected_map);
                }
                draw_frame_start();
                draw_main_menu(menu_img, single, multi, end, cursor);
                check_cursor_position(cursor, single, multi, end, game_state, &menu_state, NULL);
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_START_TO_PLAY_MULTIPLAYER) {
                set_selected(single, false);
                set_selected(multi, true);
                set_selected(end, false);
                reset_game_multiplayer(scoreboard, tb, bola, player1, player2, &ball_state, &player1_state_move, &player2_state_move, &player1_state_jump, &player2_state_jump);
                draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true);
                game_state = STATE_GAME_PLAY_MULTIPLAYER;
              }
              else if (game_state == STATE_GAME_START_TO_PLAY) {
                set_selected(single, true);
                set_selected(multi, false);
                set_selected(end, false);
                reset_game_singleplayer(scoreboard, tb, bola, player1, &ball_state, &player1_state_move, &player1_state_jump);
                draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true);
                game_state = STATE_GAME_PLAY;
              }
              else if (game_state == STATE_GAME_PAUSE_TO_START) {
                set_selected(resume, true);
                set_selected(go_back, false);
                draw_frame_start();
                draw_main_menu(menu_img, single, multi, end, cursor);
                draw_frame_end();
                game_state = STATE_GAME_START;
              }
              else if (game_state == STATE_GAME_PLAY_TO_PAUSE) {
                game_state = STATE_GAME_PAUSE;
                xpm_load(pause_menu_xpm, XPM_8_8_8, &pause_menu_img);
                draw_main_menu(pause_menu_img, resume, go_back, NULL, cursor);
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_PAUSE_TO_PLAY) {
                set_selected(resume, true);
                set_selected(go_back, false);
                if (get_selected(multi)) {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true);
                }

                if (player_get_xspeed(player1) != 0) {
                  player1_state_move = player_get_xspeed(player1) < 0 ? STATE_AFTER_PLAYER_MOVE_LEFT : STATE_AFTER_PLAYER_MOVE_RIGHT;
                }
                if (player_get_xspeed(player2) != 0) {
                  player2_state_move = player_get_xspeed(player2) < 0 ? STATE_AFTER_PLAYER_MOVE_LEFT : STATE_AFTER_PLAYER_MOVE_RIGHT;
                }
                game_state = get_selected(multi) ? STATE_GAME_PLAY_MULTIPLAYER : STATE_GAME_PLAY;
              }
              else if (game_state == STATE_GAME_PAUSE) {
                if (menu_pause_state == STATE_MENU_HOVER_RESUME && resume->selected == false) {
                  set_selected(resume, true);
                  set_selected(go_back, false);
                  set_image(resume, start_selected_map);
                  set_image(go_back, end_not_selected_map);
                }
                if (menu_pause_state == STATE_MENU_HOVER_GO_BACK && go_back->selected == false) {
                  set_selected(resume, false);
                  set_selected(go_back, true);
                  set_image(resume, start_not_selected_map);
                  set_image(go_back, end_selected_map);
                }
                if (get_selected(multi)) {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                draw_main_menu(pause_menu_img, resume, go_back, NULL, cursor);
                check_cursor_position(cursor, single, multi, NULL, game_state, NULL, &menu_pause_state);
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_END) {
                set_selected(go_back, true);
                set_image(go_back, end_selected_map);
                if (get_selected(multi)) {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                if (scoreboard->points1 > scoreboard->points2) {
                  xpm_load(player1_win_menu_xpm, XPM_8_8_8, &player1win_menu_img);
                  draw_main_menu(player1win_menu_img, NULL, go_back, NULL, cursor);
                }
                else if (scoreboard->points1 < scoreboard->points2) {
                  xpm_load(player2_win_menu_xpm, XPM_8_8_8, &player2win_menu_img);
                  draw_main_menu(player2win_menu_img, NULL, go_back, NULL, cursor);
                }
                else {
                  xpm_load(windraw_menu_xpm, XPM_8_8_8, &draw_menu_img);
                  draw_main_menu(draw_menu_img, NULL, go_back, NULL, cursor);
                }
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_TIME_REACHED) {
                xpm_load(time_reached_menu_xpm, XPM_8_8_8, &time_reached_menu_img);
                set_selected(go_back, true);
                set_image(go_back, end_selected_map);
                if (get_selected(multi)) {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                draw_main_menu(time_reached_menu_img, NULL, go_back, NULL, cursor);
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_GOAL) {
                xpm_load(goal_card_menu_xpm, XPM_8_8_8, &goal_card_menu_img);
                if (get_selected(multi)) {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true);
                }
                draw_main_menu(goal_card_menu_img, NULL, NULL, NULL, NULL);
                draw_frame_end();
                sleep(3);
                if (get_selected(multi)) {
                  restart_game(bola, &ball_state, player1, player2, &player1_state_move, &player2_state_move, &player1_state_jump, &player2_state_jump);
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  restart_game(bola, &ball_state, player1, NULL, &player1_state_move, NULL, &player1_state_jump, NULL);
                  draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true);
                }
                game_state = get_selected(multi) ? STATE_GAME_PLAY_MULTIPLAYER : STATE_GAME_PLAY;
                draw_frame_start();
                draw_xpm(bola->x, bola->y, bola->img, false);
                draw_xpm(player1->x, player1->y, player1->img, true);
                if (get_selected(multi)) {
                  draw_xpm(player2->x, player2->y, player2->img, true);
                }
                draw_frame_end();
              }
              else if (game_state == STATE_GAME_PLAY) {
                if (ball_state != STATE_NONE) {
                  draw_frame_start();
                }
                if (player1_state_move != STATE_PLAYER_MOVE_NONE || player2_state_move != STATE_PLAYER_MOVE_NONE) {
                  draw_frame_start();
                }
                if(check_player_border(player1)){
                  if(player1_state_move != STATE_PLAYER_MOVE_NONE){
                    player1_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                if(check_border(bola, player1) && !touching1){
                  if(player1->x + player1->width/2 > bola->x + bola->width/2){
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_LEFT_START;
                    }else{
                      ball_state = STATE_MOVE_LEFT_START;
                    }
                  }else{
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_RIGHT_START;
                    }else{
                      ball_state = STATE_MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if(!check_border(bola, player1) && touching1){
                  ball_state = STATE_AFTER_MOVE;
                  touching1 = false;
                }
                if(check_kicking_player1(bola, player1)){
                  if(ball_state != STATE_NONE){
                    ball_state = STATE_JUMP_END;
                    ball_state_temporary = STATE_START_JUMP_RIGHT;
                  }else{
                    ball_state = STATE_START_JUMP_RIGHT;
                  }
                }
                move_ball(bola, &ball_state, &ball_state_temporary, player1);
                ball_goal_collision(bola, goal_, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right_, scoreboard, &ball_state);
                add_time(tb);
                draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, NULL, goal_img, goal_right_img, true);
                if (tb->time == 0) {
                  game_state = STATE_GAME_TIME_REACHED;
                }
                if (bola->stop) {
                  game_state = STATE_GAME_GOAL;
                }
              }
              else if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (ball_state != STATE_NONE) {
                  draw_frame_start();
                }
                if (player1_state_move != STATE_PLAYER_MOVE_NONE || player2_state_move != STATE_PLAYER_MOVE_NONE) {
                  draw_frame_start();
                }
                if(check_player_border(player1)){
                  if(player1_state_move != STATE_PLAYER_MOVE_NONE){
                    player1_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                if(check_player_border(player2)){
                  if(player2_state_move != STATE_PLAYER_MOVE_NONE){
                    player2_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                if(check_multiplayer_border(player1, player2)){
                  if(player1_state_move != STATE_PLAYER_MOVE_NONE){
                    player1_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
                  }
                  if(player2_state_move != STATE_PLAYER_MOVE_NONE){
                    player2_state_move = STATE_AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                move_player(player2, &player2_state_move, &player2_state_move_temporary, &player2_state_jump, &player2_state_jump_temporary, &player2_state_kick, &player2_state_kick_temporary);
                if(check_border(bola, player1) && !touching1){
                  if(player1->x > bola->x + bola->width/2){
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_LEFT_START;
                    }else{
                      ball_state = STATE_MOVE_LEFT_START;
                    }
                  }else{
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_RIGHT_START;
                    }else{
                      ball_state = STATE_MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if(!check_border(bola, player1) && touching1){
                  ball_state = STATE_AFTER_MOVE;
                  touching1 = false;
                }
                if(check_border(bola, player2) && !touching2){
                  if(player2->x > bola->x + bola->width/2){
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_LEFT_START;
                    }else{
                      ball_state = STATE_MOVE_LEFT_START;
                    }
                  }else{
                    if(ball_state != STATE_NONE){
                      ball_state = STATE_JUMP_END;
                      ball_state_temporary = STATE_MOVE_RIGHT_START;
                    }else{
                      ball_state = STATE_MOVE_RIGHT_START;
                    }
                  }
                  touching2 = true;
                }
                else if(!check_border(bola, player2) && touching2){
                  ball_state = STATE_AFTER_MOVE;
                  touching2 = false;
                }
                move_ball(bola, &ball_state, &ball_state_temporary, player1);
                ball_goal_collision(bola, goal_, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right_, scoreboard, &ball_state);
                add_time(tb);
                draw_game(muro_, goal_, goal_right_, scoreboard, tb, bola, player1, player2, goal_img, goal_right_img, true);
                if (scoreboard->points1 == 5 || scoreboard->points2 == 5 || tb->time == 0) {
                  game_state = STATE_GAME_END;
                }
                if (bola->stop) {
                  game_state = STATE_GAME_GOAL;
                }
              }
            }
          }
          if ((msg.m_notify.interrupts & keyboard_int_bit) && keyboard_active) { // subscribed keyboard interrupt
            kbc_ih();
            uint8_t scancode = get_scancode();
            bool make = 1;
            if (scancode & BREAKCODE) {
              make = 0;
            }

            if (scancode == SCANCODE_E0) { // 0xe0 has continue to prevent wasting time seeing conditions
              continue;
            }

            if (kbd_print_scancode(make, 1, &scancode)) {
              return 1;
            }

            if (MAKECODE_RIGHT == scancode) { // 0x4d is the makecode for the right key
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player1_state_move != STATE_PLAYER_MOVE_NONE) {
                  //player_state_move = STATE_PLAYER_MOVE_END;
                  //player_state_move_temporary = STATE_PLAYER_MOVE_RIGHT_START;
                }else{
                  if(player1_state_jump != STATE_PLAYER_JUMP_NONE){
                     player1_state_move = STATE_PLAYER_MOVE_RIGHT;
                  }else{
                    player1_state_move = STATE_PLAYER_MOVE_RIGHT_START;
                  }
                }
              }
            }

            if (MAKECODE_LEFT == scancode) { // 0x4b is the makecode for the left key
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player1_state_move == STATE_PLAYER_MOVE_NONE) {
                  if (player1_state_jump != STATE_PLAYER_JUMP_NONE) {
                    player1_state_move = STATE_PLAYER_MOVE_LEFT;
                  }
                  else {
                    player1_state_move = STATE_PLAYER_MOVE_LEFT_START;
                  }
                }
              }
            }

            if (MAKECODE_A == scancode) { // 0x4b is the makecode for the A key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player2_state_move == STATE_PLAYER_MOVE_NONE) {
                  if (player2_state_jump != STATE_PLAYER_JUMP_NONE) {
                    player2_state_move = STATE_PLAYER_MOVE_LEFT;
                  }
                  else {
                    player2_state_move = STATE_PLAYER_MOVE_LEFT_START;
                  }
                }
                if(check_border(bola, player2)){
                  if (ball_state != STATE_NONE) {
                    ball_state = STATE_JUMP_END;
                    ball_state_temporary = STATE_MOVE_RIGHT_START;
                  }else{
                    ball_state = STATE_MOVE_RIGHT_START;
                  }
                }
              }
            }

            if (MAKECODE_D == scancode) { // 0x20 is the makecode for the D key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player2_state_move == STATE_PLAYER_MOVE_NONE) {
                  if (player2_state_jump != STATE_PLAYER_JUMP_NONE) {
                    player2_state_move = STATE_PLAYER_MOVE_RIGHT;
                  }
                  else {
                    player2_state_move = STATE_PLAYER_MOVE_RIGHT_START;
                  }
                }
                if(check_border(bola, player2)){
                  if (ball_state != STATE_NONE) {
                    ball_state = STATE_JUMP_END;
                    ball_state_temporary = STATE_MOVE_LEFT_START;
                  }else{
                    ball_state = STATE_MOVE_LEFT_START;
                  }
                }
              }
            }

            if (BREAKCODE_LEFT == scancode) { // 0xcd is the breakcode for the right key
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                player1_state_move = STATE_PLAYER_MOVE_LEFT_END;
              }
            }

            if (BREAKCODE_RIGHT == scancode) { // 0xcb is the breakcode for the left key
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                player1_state_move = STATE_PLAYER_MOVE_RIGHT_END;
              }
            }

            if (BREAKCODE_A == scancode) { // 0x9e is the breakcode for the A key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                player2_state_move = STATE_PLAYER_MOVE_LEFT_END;
              }
            }

            if (BREAKCODE_D == scancode) { // 0xa0 is the breakcode for the D key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                player2_state_move = STATE_PLAYER_MOVE_RIGHT_END;
              }
            }

            if (MAKECODE_W == scancode) { // 0x11 is the makecode for the W key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player2_state_jump == STATE_PLAYER_JUMP_NONE) {
                  player2_state_jump = STATE_START_PLAYER_JUMP;
                }
              }
            }

            if (MAKECODE_S == scancode) { // 0x1f is the makecode for the S key
              if (game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player2_state_kick == STATE_PLAYER_KICK_NONE) {
                  player2_state_kick = STATE_PLAYER_KICK_START;
                }
              }
            }

            if (MAKECODE_UP == scancode) { // 0x48 is the makecode for the up key
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player1_state_jump == STATE_PLAYER_JUMP_NONE) {
                  player1_state_jump = STATE_START_PLAYER_JUMP;
                }
              }
              else if (game_state == STATE_GAME_START) {
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER) {
                  menu_state = STATE_MENU_HOVER_EXIT;
                }
                else if (menu_state == STATE_MENU_HOVER_MULTIPLAYER) {
                  menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
                }
                else if (menu_state == STATE_MENU_HOVER_EXIT) {
                  menu_state = STATE_MENU_HOVER_MULTIPLAYER;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }
              else if (game_state == STATE_GAME_PAUSE) {
                if (menu_pause_state == STATE_MENU_HOVER_RESUME) {
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }
                else if (menu_pause_state == STATE_MENU_HOVER_GO_BACK) {
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }
            }

            if (MAKECODE_DOWN == scancode) { // 0x50 is the makecode for the down key
              if (game_state == STATE_GAME_START) {
                if (menu_state == STATE_MENU_HOVER_SINGLEPLAYER) {
                  menu_state = STATE_MENU_HOVER_MULTIPLAYER;
                }
                else if (menu_state == STATE_MENU_HOVER_MULTIPLAYER) {
                  menu_state = STATE_MENU_HOVER_EXIT;
                }
                else if (menu_state == STATE_MENU_HOVER_EXIT) {
                  menu_state = STATE_MENU_HOVER_SINGLEPLAYER;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }
              else if (game_state == STATE_GAME_PAUSE) {
                if (menu_pause_state == STATE_MENU_HOVER_RESUME) {
                  menu_pause_state = STATE_MENU_HOVER_GO_BACK;
                }
                else if (menu_pause_state == STATE_MENU_HOVER_GO_BACK) {
                  menu_pause_state = STATE_MENU_HOVER_RESUME;
                }
              }else if(game_state == STATE_GAME_PAUSE){
                if (menu_pause_state == STATE_MENU_HOVER_RESUME){
                  game_state = STATE_GAME_PAUSE_TO_PLAY;
                }else if(menu_pause_state == STATE_MENU_HOVER_GO_BACK){
                  game_state = STATE_GAME_PAUSE_TO_START;
                }
              }
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (player1_state_kick == STATE_PLAYER_KICK_NONE) {
                  player1_state_kick = STATE_PLAYER_KICK_START;
                }else{
                  player1_state_kick = STATE_AFTER_PLAYER_KICK;
                  player1_state_kick_temporary = STATE_PLAYER_KICK_START;
                }
                if(check_kicking_player1(bola, player1)){
                  if (ball_state != STATE_NONE) {
                    ball_state = STATE_JUMP_END;
                    ball_state_temporary = STATE_START_JUMP_RIGHT;
                  }else{
                    ball_state = STATE_START_JUMP_RIGHT;
                  }
                }
              }
            }

            if (MAKECODE_ENTER == scancode) { // 0x50 is the makecode for the enter key
              enter_new_state(&game_state, &menu_state, &menu_pause_state, &done);
            }

            if (BREAKCODE_ESC == scancode) {
              if (game_state == STATE_GAME_PLAY || game_state == STATE_GAME_PLAY_MULTIPLAYER) {
                if (game_state != STATE_GAME_END && game_state != STATE_GAME_TIME_REACHED && bola->stop == false && game_state != STATE_GAME_GOAL) {
                  game_state = STATE_GAME_PLAY_TO_PAUSE;
                }
              }
            }
          }
          if ((msg.m_notify.interrupts & mouse_int_bit) && mouse_active) { // subscribed mouse interrupt
            mouse_ih();
            uint8_t packetByte = get_packetByte();
            if (counter_byte_packet == 0 && !(packetByte & BIT(3))) {
              printf("ERROR: This packet's first byte has bit 3 at 0!\n");
              continue;
            }

            mouse_big_packet[counter_byte_packet] = packetByte;
            counter_byte_packet++;

            if (counter_byte_packet == 3) {
              // bool clicked_rb = mouse_big_packet[0] & PS2_RB;
              // bool clicked_mb = mouse_big_packet[0] & PS2_MB;
              bool clicked_lb = mouse_big_packet[0] & PS2_LB;

              bool exists_x_ovf = mouse_big_packet[0] & PS2_XOVER;
              bool exists_y_ovf = mouse_big_packet[0] & PS2_YOVER;

              if (!exists_x_ovf && !exists_y_ovf) {

                int16_t delta_x;
                int16_t delta_y;

                if (mouse_big_packet[0] & PS2_XDELTA) { // bit 4
                  uint16_t x = 0xFF00 | mouse_big_packet[1];
                  delta_x = ((~x) + 1);
                }
                else {
                  delta_x = mouse_big_packet[1];
                }

                if ((mouse_big_packet[0] & PS2_YDELTA)) { // bit 5
                  uint16_t y = 0xFF00 | mouse_big_packet[2];
                  delta_y = -((~y) + 1);
                }
                else {
                  delta_y = mouse_big_packet[2];
                }

                // mouse_print_packet(&big_packet_to_print);
                set_coordinates(cursor, delta_x % 800, delta_y % 600, mouse_big_packet[0] & PS2_XDELTA, !(mouse_big_packet[0] & PS2_YDELTA));
                /*printf("delat x: %d\n", delta_x);
                printf("delat y: %d\n", delta_y);
                printf("Cursor x: %d\n", cursor->x);
                printf("Cursor y: %d\n", cursor->y);*/

                if (check_cursor_position(cursor, single, multi, end, game_state, &menu_state, &menu_pause_state) && clicked_lb) {
                  enter_new_state(&game_state, &menu_state, &menu_pause_state, &done);
                }
              }

              counter_byte_packet = 0;
            }
          }
          if (msg.m_notify.interrupts & rtc_int_bit) { // subscribed mouse interrupt
            rtc_ih();

            // For debugging: Only works if struct is public
            // rtc_data_t date_time = get_current_time();
            // printf("Today is: %d/%d/%d, %d. Current time is %d:%d:%d\n", date_time.day, date_time.month, date_time.year, date_time.weekday, date_time.hours, date_time.minutes, date_time.seconds);
          }
          if (msg.m_notify.interrupts & uart_int_bit) { // subscribed mouse interrupt
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
  destroy_player(player1);
  destroy_player(player2);
  destroy_scoreboard(scoreboard);
  destroy_timeboard(tb);

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

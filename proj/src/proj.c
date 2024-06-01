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

#include "game_state.h"

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

// ======= Define Initial States =======
static GameState game_state = MENU;
static MenuState menu_state = HOVER_SINGLEPLAYER;
static MenuPauseState menu_pause_state = HOVER_RESUME;

static BallState ball_state = STATE_NONE;
static BallState ball_state_temporary = STATE_NONE;

static PlayerStateJump player1_state_jump = PLAYER_JUMP_NONE;
static PlayerStateJump player1_state_jump_temporary = PLAYER_JUMP_NONE;
static PlayerStateMove player1_state_move = PLAYER_MOVE_NONE;
static PlayerStateMove player1_state_move_temporary = PLAYER_MOVE_NONE;
static PlayerStateKick player1_state_kick = PLAYER_KICK_NONE;
static PlayerStateKick player1_state_kick_temporary = PLAYER_KICK_NONE;

static PlayerStateJump player2_state_jump = PLAYER_JUMP_NONE;
static PlayerStateJump player2_state_jump_temporary = PLAYER_JUMP_NONE;
static PlayerStateMove player2_state_move = PLAYER_MOVE_NONE;
static PlayerStateMove player2_state_move_temporary = PLAYER_MOVE_NONE;
static PlayerStateKick player2_state_kick = PLAYER_KICK_NONE;
static PlayerStateKick player2_state_kick_temporary = PLAYER_KICK_NONE;

// ====== Define XPMs =======
static xpm_map_t player1_map0 = (xpm_map_t) personagem_parado_xpm;
static xpm_map_t player2_map0 = (xpm_map_t) personagem_2_parado_xpm;
static xpm_map_t ground_map = (xpm_map_t) muro_xpm;
static xpm_map_t bola_map = (xpm_map_t) bola_xpm;
static xpm_map_t goal_map = (xpm_map_t) goal_back_xpm;
static xpm_map_t goal_right_map = (xpm_map_t) baliza_tras_right_xpm;
static xpm_map_t goal_front_map = (xpm_map_t) goal_front_xpm;
static xpm_map_t goal_right_front_map = (xpm_map_t) goal_front_right_xpm;
static xpm_map_t scoreboard_map = (xpm_map_t) scoreboard_90_xpm;
static xpm_map_t tb_map = (xpm_map_t) timeboard_90_xpm;

static xpm_map_t cursor_map = (xpm_map_t) cursor_xpm;
static xpm_map_t start_selected_map = (xpm_map_t) play_xpm;
static xpm_map_t start_not_selected_map = (xpm_map_t) play_sem_xpm;
static xpm_map_t end_selected_map = (xpm_map_t) close_xpm;
static xpm_map_t end_not_selected_map = (xpm_map_t) close_sem_xpm;
static xpm_map_t menu_map = (xpm_map_t) menu_xpm;
static xpm_map_t pause_menu_map = (xpm_map_t) pausa_menu_90_sem_background_xpm;
static xpm_map_t player1win_menu_map = (xpm_map_t) player1win_menu_xpm;
static xpm_map_t player2win_menu_map = (xpm_map_t) player2win_menu_xpm;
static xpm_map_t draw_menu_map = (xpm_map_t) draw_menu_xpm;
static xpm_map_t time_reached_menu_map = (xpm_map_t) timereached_menu_xpm;
static xpm_map_t goal_card_menu_map = (xpm_map_t) goal_card_xpm;

// ======== Objects ========
static Cursor *cursor;
static Ball *bola;
static Player *player1;
static Player *player2;
static Button *single;
static Button *multi;
static Button *end;
static Button *resume;
static Button *go_back;
static Wall *muro;
static Goal *goal;
static Goal *goal_right;
static Scoreboard *scoreboard;
static Timeboard *timeboard;


int interrupt_loop() {
  return 0;
}

int keyboard_active = 1;
int mouse_active = 1;

uint8_t mouse_big_packet[3];
uint8_t counter_byte_packet = 0;

void enter_new_state(GameState *game_state, MenuState *menu_state, MenuPauseState *menu_pause_state, bool *done) {
  if (*game_state == MENU) {
    if (*menu_state == HOVER_SINGLEPLAYER) {
      *game_state = PREPARE_SINGLEPLAYER;
    }
    else if (*menu_state == HOVER_MULTIPLAYER) {
      *game_state = PREPARE_MULTIPLAYER;
    }
    else if (*menu_state == HOVER_EXIT) {
      *game_state = STATE_GAME_OVER;
      *done = true;
    }
  }
  else if (*game_state == PAUSE) {
    if (*menu_pause_state == HOVER_RESUME) {
      *game_state = PAUSE_TO_PLAY;
    }
    else if (*menu_pause_state == HOVER_GO_BACK) {
      *game_state = PAUSE_TO_MENU;
    }
  }
  else if (*game_state == STATE_GAME_END || *game_state == TIME_LIMIT) {
    *game_state = PAUSE_TO_MENU;
  }
}

bool is_cursor_touching(Cursor *cursor, Button *button) {
  uint16_t cursor_end = cursor->x + cursor->img.width;
  uint16_t cursor_bottom = cursor->y + cursor->img.height;
  uint16_t button_end = button->x + button->img.width;
  uint16_t button_bottom = button->y + button->img.height;
  return !((cursor_bottom < button->y || button_bottom < cursor->y) || (cursor_end < button->x || button_end < cursor->x));
}

bool check_cursor_position(Cursor *cursor, Button *single, Button *multi, Button *end, GameState game_state, MenuState *menu_state, MenuPauseState *menu_pause_state) {

  bool is_touching_single = (single != NULL) ? is_cursor_touching(cursor, single) : false;
  bool is_touching_multi = (multi != NULL) ? is_cursor_touching(cursor, multi) : false;
  bool is_touching_end = (end != NULL) ? is_cursor_touching(cursor, end) : false;

  if (is_touching_single) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_SINGLEPLAYER;
    }
    else if (game_state == PAUSE && menu_pause_state != NULL) {
      *menu_pause_state = HOVER_RESUME;
    }
  }
  if (is_touching_multi) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_MULTIPLAYER;
    }
    else if (game_state == PAUSE && menu_pause_state != NULL) {
      *menu_pause_state = HOVER_GO_BACK;
    }
  }
  if (is_touching_end) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_EXIT;
    }
  }
  return is_touching_single || is_touching_multi || is_touching_end;
}

void reset_states(BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump, bool multiplayer) {
  if (*ball_state != STATE_NONE) {
    *ball_state = JUMP_END;
  }
  else {
    *ball_state = STATE_NONE;
  }
  if (*player1_state_move != PLAYER_MOVE_NONE) {
    *player1_state_move = PLAYER_MOVE_RIGHT_END;
    *player1_state_jump = PLAYER_JUMP_END;
  }
  else {
    *player1_state_move = PLAYER_MOVE_NONE;
    *player1_state_jump = PLAYER_JUMP_NONE;
  }

  if (multiplayer && player2_state_move != NULL && player2_state_jump != NULL) {
    if (*player2_state_move != PLAYER_MOVE_NONE) {
      *player2_state_move = PLAYER_MOVE_RIGHT_END;
      *player2_state_jump = PLAYER_JUMP_END;
    }
    else {
      *player2_state_move = PLAYER_MOVE_NONE;
      *player2_state_jump = PLAYER_JUMP_NONE;
    }
  }
}

int reset_game_multiplayer(Scoreboard *scoreboard, Timeboard *timeboard, Ball *bola, Player *player1, Player *player2, BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump) {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  timeboard->time = 300;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  move_player2_to_center(player2);
  reset_states(ball_state, player1_state_move, player2_state_move, player1_state_jump, player2_state_jump, player2 != NULL);
  return 0;
}

int reset_game_singleplayer(Scoreboard *scoreboard, Timeboard *timeboard, Ball *bola, Player *player1, BallState *ball_state, PlayerStateMove *player1_state_move, PlayerStateJump *player1_state_jump) {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  timeboard->time = 600;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  reset_states(ball_state, player1_state_move, NULL, player1_state_jump, NULL, false);
  return 0;
}

int draw_game(Wall *muro, Goal *goal, Goal *goal_right, Scoreboard *scoreboard, Timeboard *timeboard, Ball *bola, Player *player1, Player *player2, xpm_image_t goal_img, xpm_image_t goal_right_img, bool done) {

  if (draw_back(0, 600 - 314, muro->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(goal->x, goal->y, goal->img)) {
    return 1; // epa nao deu pra desenhar background
  }

  if (draw_back(goal_right->x, goal_right->y, goal_right->img)) {
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

  if (draw_back(timeboard->x, timeboard->y, timeboard->img)) {
    return 1;
  }

  int minutes = timeboard->time / 60;
  int seconds = timeboard->time % 60;

  // To draw the minutes in the timeboard
  draw_back_scoreboard(timeboard->x + 15, timeboard->y + timeboard->img.height / 3, minutes / 10);
  draw_back_scoreboard(timeboard->x + 35, timeboard->y + timeboard->img.height / 3, minutes % 10);

  // To draw the seconds in the timeboard
  draw_back_scoreboard(timeboard->x + 80, timeboard->y + timeboard->img.height / 3, seconds / 10);
  draw_back_scoreboard(timeboard->x + 100, timeboard->y + timeboard->img.height / 3, seconds % 10);

  draw_frame_start();

  // To draw characters from the game
  draw_xpm(bola->x, bola->y, bola->img, false);

  draw_xpm(player1->x, player1->y, player1->img, true);
  if (player2 != NULL) {
    draw_xpm(player2->x, player2->y, player2->img, true);
  }

  draw_xpm(goal->x - 8, goal->y - 18, goal_img, true);
  draw_xpm(goal_right->x, goal_right->y - 18, goal_right_img, true);

  if (done)
    draw_frame_end();
  return 0;
}

void draw_main_menu(xpm_image_t menu_img, Button *single, Button *multi, Button *end, Cursor *cursor_) {
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

void restart_game(Ball *bl, BallState *ball_state, Player *player1, Player *player2, PlayerStateMove *player1_state_move, PlayerStateMove *player2_state_move, PlayerStateJump *player1_state_jump, PlayerStateJump *player2_state_jump) {
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
  bool done = false, touching1 = false, touching2 = false, kicking = false, kicking2 = false;

  // Loading images without a class
  xpm_image_t goal_img;
  xpm_image_t goal_right_img;
  xpm_load(goal_front_map, XPM_8_8_8, &goal_img);
  xpm_load(goal_right_front_map, XPM_8_8_8, &goal_right_img);
 
  xpm_image_t menu_img;
  xpm_image_t pause_menu_img;
  xpm_image_t player1win_menu_img;
  xpm_image_t player2win_menu_img;
  xpm_image_t draw_menu_img;
  xpm_image_t time_reached_menu_img;
  xpm_image_t goal_card_menu_img;
  xpm_load(menu_map, XPM_8_8_8, &menu_img);
  xpm_load(pause_menu_map, XPM_8_8_8, &pause_menu_img);
  xpm_load(player1win_menu_map, XPM_8_8_8, &player1win_menu_img);
  xpm_load(player2win_menu_map, XPM_8_8_8, &player2win_menu_img);
  xpm_load(draw_menu_map, XPM_8_8_8, &draw_menu_img);
  xpm_load(time_reached_menu_map, XPM_8_8_8, &time_reached_menu_img);
  xpm_load(goal_card_menu_map, XPM_8_8_8, &goal_card_menu_img);


  // ======= Loading objects =======
  cursor = create_cursor(cursor_map);
  single = create_button(start_selected_map, 500, 155, true);
  multi = create_button(start_not_selected_map, 500, 219, false);
  end = create_button(end_not_selected_map, 500, 284, false);
  resume = create_button(start_selected_map, 500, 155, true);
  go_back = create_button(end_not_selected_map, 500, 219, false);

  bola = create_ball(bola_map, 400, 490, 32, 32, 10, 50, 0);
  player1 = create_player(player1_map0, 200, 450, 62, 78, 10, 50, 0);
  player2 = create_player(player2_map0, 600, 450, 62, 78, 10, 50, 1);
  muro = create_wall(ground_map);
  goal = create_goal(goal_map, 0);
  goal_right = create_goal(goal_right_map, 1);
  scoreboard = create_scoreboard(scoreboard_map);
  timeboard = create_timeboard(tb_map);


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


  if (game_state == MENU) {
    draw_frame_start();
    draw_main_menu(menu_img, single, multi, end, cursor);
    draw_frame_end();
  }

  if (game_state == SINGLEPLAYER) {
    if (draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true)) {
      return 1;
    }
  }
  if (game_state == MULTIPLAYER) {
    if (draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true)) {
      return 1;
    }
  }


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

            if (get_timer_intCounter() % 2 == 0) { // 30 fps
              if (game_state == MENU) {
                if (menu_state == HOVER_SINGLEPLAYER && single->selected == false) {
                  set_selected(single, true);
                  set_selected(multi, false);
                  set_selected(end, false);
                  set_image(single, start_selected_map);
                  set_image(multi, start_not_selected_map);
                  set_image(end, end_not_selected_map);
                }
                if (menu_state == HOVER_MULTIPLAYER && multi->selected == false) {
                  set_selected(single, false);
                  set_selected(multi, true);
                  set_selected(end, false);
                  set_image(single, start_not_selected_map);
                  set_image(multi, start_selected_map);
                  set_image(end, end_not_selected_map);
                }
                else if (menu_state == HOVER_EXIT && end->selected == false) {
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
              else if (game_state == PREPARE_SINGLEPLAYER) {
                set_selected(single, true);
                set_selected(multi, false);
                set_selected(end, false);
                reset_game_singleplayer(scoreboard, timeboard, bola, player1, &ball_state, &player1_state_move, &player1_state_jump);
                draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true);
                game_state = SINGLEPLAYER;
              }
              else if (game_state == PREPARE_MULTIPLAYER) {
                set_selected(single, false);
                set_selected(multi, true);
                set_selected(end, false);
                reset_game_multiplayer(scoreboard, timeboard, bola, player1, player2, &ball_state, &player1_state_move, &player2_state_move, &player1_state_jump, &player2_state_jump);
                draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true);
                game_state = MULTIPLAYER;
              }
              else if (game_state == PREPARE_PAUSE) {
                game_state = PAUSE;
                draw_main_menu(pause_menu_img, resume, go_back, NULL, cursor);
                draw_frame_end();
              }
              else if (game_state == PAUSE) {
                if (menu_pause_state == HOVER_RESUME && resume->selected == false) {
                  set_selected(resume, true);
                  set_selected(go_back, false);
                  set_image(resume, start_selected_map);
                  set_image(go_back, end_not_selected_map);
                }
                if (menu_pause_state == HOVER_GO_BACK && go_back->selected == false) {
                  set_selected(resume, false);
                  set_selected(go_back, true);
                  set_image(resume, start_not_selected_map);
                  set_image(go_back, end_selected_map);
                }
                if (get_selected(multi)) {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                draw_main_menu(pause_menu_img, resume, go_back, NULL, cursor);
                check_cursor_position(cursor, single, multi, NULL, game_state, NULL, &menu_pause_state);
                draw_frame_end();
              }
              else if (game_state == PAUSE_TO_PLAY) {
                set_selected(resume, true);
                set_selected(go_back, false);
                if (get_selected(multi)) {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true);
                }

                if (player_get_xspeed(player1) != 0) {
                  player1_state_move = player_get_xspeed(player1) < 0 ? AFTER_PLAYER_MOVE_LEFT : AFTER_PLAYER_MOVE_RIGHT;
                }
                if (player_get_xspeed(player2) != 0) {
                  player2_state_move = player_get_xspeed(player2) < 0 ? AFTER_PLAYER_MOVE_LEFT : AFTER_PLAYER_MOVE_RIGHT;
                }
                game_state = get_selected(multi) ? MULTIPLAYER : SINGLEPLAYER;
              }
              else if (game_state == PAUSE_TO_MENU) {
                set_selected(resume, true);
                set_selected(go_back, false);
                draw_frame_start();
                draw_main_menu(menu_img, single, multi, end, cursor);
                draw_frame_end();
                game_state = MENU;
              }
              else if (game_state == STATE_GAME_END) {
                set_selected(go_back, true);
                set_image(go_back, end_selected_map);
                if (get_selected(multi)) {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                if (scoreboard->points1 > scoreboard->points2) {
                  draw_main_menu(player1win_menu_img, NULL, go_back, NULL, cursor);
                }
                else if (scoreboard->points1 < scoreboard->points2) {
                  draw_main_menu(player2win_menu_img, NULL, go_back, NULL, cursor);
                }
                else {
                  draw_main_menu(draw_menu_img, NULL, go_back, NULL, cursor);
                }
                draw_frame_end();
              }
              else if (game_state == TIME_LIMIT) {
                set_selected(go_back, true);
                set_image(go_back, end_selected_map);
                if (get_selected(multi)) {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, false);
                }
                else {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, false);
                }
                draw_main_menu(time_reached_menu_img, NULL, go_back, NULL, cursor);
                draw_frame_end();
              }
              else if (game_state == GOAL) {
                if (get_selected(multi)) {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true);
                }
                draw_main_menu(goal_card_menu_img, NULL, NULL, NULL, NULL);
                draw_frame_end();
                sleep(1);
                if (get_selected(multi)) {
                  restart_game(bola, &ball_state, player1, player2, &player1_state_move, &player2_state_move, &player1_state_jump, &player2_state_jump);
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true);
                }
                else {
                  restart_game(bola, &ball_state, player1, NULL, &player1_state_move, NULL, &player1_state_jump, NULL);
                  draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true);
                }
                game_state = get_selected(multi) ? MULTIPLAYER : SINGLEPLAYER;
                draw_frame_start();
                draw_xpm(bola->x, bola->y, bola->img, false);
                draw_xpm(player1->x, player1->y, player1->img, true);
                if (get_selected(multi)) {
                  draw_xpm(player2->x, player2->y, player2->img, true);
                }
                draw_frame_end();
              }
              else if (game_state == SINGLEPLAYER) {
                if (ball_state != STATE_NONE) {
                  draw_frame_start();
                }
                if (player1_state_move != PLAYER_MOVE_NONE || player2_state_move != PLAYER_MOVE_NONE) {
                  draw_frame_start();
                }
                if (check_player_border(player1)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                if (check_border(bola, player1) && !touching1) {
                  if (player1->x + player1->width/2 > bola->x + bola->width/2) {
                    if (ball_state != START_JUMP_LEFT && ball_state != START_JUMP_RIGHT && ball_state != JUMP_LEFT && ball_state != JUMP_RIGHT && ball_state != BEFORE_JUMP_LEFT && ball_state != BEFORE_JUMP_RIGHT && ball_state != AFTER_JUMP_LEFT && ball_state != AFTER_JUMP_RIGHT){
                      if(ball_state != STATE_NONE){
                        ball_state = JUMP_END;
                        ball_state_temporary = MOVE_LEFT_START;
                      }else{
                        ball_state = MOVE_LEFT_START;
                      }
                    }
                  }else {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    }else {
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if (!check_border(bola, player1) && touching1) {
                  ball_state = AFTER_MOVE;
                  touching1 = false;
                }
                if(check_kicking_player1(bola, player1) && kicking){
                  printf("kick\n");
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = START_JUMP_RIGHT;
                  }
                  else {
                    ball_state = START_JUMP_RIGHT;
                  }
                  kicking = false;

                }
                move_ball(bola, &ball_state, &ball_state_temporary, player1);
                ball_goal_collision(bola, goal, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right, scoreboard, &ball_state);
                decrease_time(timeboard);
                draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, NULL, goal_img, goal_right_img, true);
                if (scoreboard->points1 == 9 || scoreboard->points2 == 9 || timeboard->time == 0) {
                  game_state = TIME_LIMIT;
                }
                if (bola->stop) {
                  game_state = GOAL;
                }
              }
              else if (game_state == MULTIPLAYER) {
                if (ball_state != STATE_NONE) {
                  draw_frame_start();
                }
                if (player1_state_move != PLAYER_MOVE_NONE || player2_state_move != PLAYER_MOVE_NONE) {
                  draw_frame_start();
                }
                if (check_player_border(player1)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                if (check_player_border(player2)) {
                  if (player2_state_move != PLAYER_MOVE_NONE) {
                    player2_state_move = AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                if (check_multiplayer_border(player1, player2)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = AFTER_PLAYER_MOVE_LEFT;
                  }
                  if (player2_state_move != PLAYER_MOVE_NONE) {
                    player2_state_move = AFTER_PLAYER_MOVE_LEFT;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                move_player(player2, &player2_state_move, &player2_state_move_temporary, &player2_state_jump, &player2_state_jump_temporary, &player2_state_kick, &player2_state_kick_temporary);
                if (check_border(bola, player1) && !touching1) {
                  if (player1->x > bola->x + bola->width/2) {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_LEFT_START;
                    }else {
                      ball_state = MOVE_LEFT_START;
                    }
                  }else {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    }else {
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if (!check_border(bola, player1) && touching1) {
                  ball_state = AFTER_MOVE;
                  touching1 = false;
                }
                if (check_border(bola, player2) && !touching2) {
                  if (player2->x > bola->x + bola->width/2) {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_LEFT_START;
                    }else {
                      ball_state = MOVE_LEFT_START;
                    }
                  }else {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    }else {
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching2 = true;
                }
                else if (!check_border(bola, player2) && touching2) {
                  ball_state = AFTER_MOVE;
                  touching2 = false;
                }
                if(check_kicking_player1(bola, player1) && kicking){
                  printf("kick\n");
                  if(ball_state != STATE_NONE){
                    ball_state = JUMP_END;
                    ball_state_temporary = START_JUMP_RIGHT;
                  }else{
                    ball_state = START_JUMP_RIGHT;
                  }
                  kicking = false;
                }
                if(check_kicking_player2(bola, player2) && kicking2){
                  printf("kick\n");
                  if(ball_state != STATE_NONE){
                    ball_state = JUMP_END;
                    ball_state_temporary = START_JUMP_LEFT;
                  }else{
                    ball_state = START_JUMP_LEFT;
                  }
                  kicking2 = false;
                }
                move_ball(bola, &ball_state, &ball_state_temporary, player1);
                ball_goal_collision(bola, goal, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right, scoreboard, &ball_state);
                decrease_time(timeboard);
                draw_game(muro, goal, goal_right, scoreboard, timeboard, bola, player1, player2, goal_img, goal_right_img, true);
                if (scoreboard->points1 == 5 || scoreboard->points2 == 5 || timeboard->time == 0) {
                  game_state = STATE_GAME_END;
                }
                if (bola->stop) {
                  game_state = GOAL;
                }
              }
            }
          }
          if ((msg.m_notify.interrupts & keyboard_int_bit) && keyboard_active) { 
            // subscribed keyboard interrupt
            kbc_ih();

            uint8_t scancode = get_scancode();
            bool make = !(scancode & BREAKCODE);

            if (scancode == SCANCODE_E0) {
              continue;
            }

            if (kbd_print_scancode(make, 1, &scancode)) {
              return 1;
            }

            // PLAYER 2 CONTROLS
            if (scancode == MAKECODE_LEFT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (player1_state_move == PLAYER_MOVE_NONE) {
                  if (player1_state_jump != PLAYER_JUMP_NONE) {
                    player1_state_move = PLAYER_MOVE_LEFT;
                  }
                  else {
                    player1_state_move = PLAYER_MOVE_LEFT_START;
                  }
                }
              }
            }

            if (scancode == MAKECODE_RIGHT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (player1_state_move != PLAYER_MOVE_NONE) {
                  //player_state_move = STATE_PLAYER_MOVE_END;
                  //player_state_move_temporary = PLAYER_MOVE_RIGHT_START;
                }else {
                  if (player1_state_jump != PLAYER_JUMP_NONE) {
                     player1_state_move = PLAYER_MOVE_RIGHT;
                  }else {
                    player1_state_move = PLAYER_MOVE_RIGHT_START;
                  }
                }
              }
            }

            if (scancode == BREAKCODE_LEFT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                player1_state_move = PLAYER_MOVE_LEFT_END;
              }
            }

            if (scancode == BREAKCODE_RIGHT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                player1_state_move = PLAYER_MOVE_RIGHT_END;
              }
            }

            // PLAYER 1 CONTROLS
            if (scancode == MAKECODE_A) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_move == PLAYER_MOVE_NONE) {
                  if (player2_state_jump != PLAYER_JUMP_NONE) {
                    player2_state_move = PLAYER_MOVE_LEFT;
                  }
                  else {
                    player2_state_move = PLAYER_MOVE_LEFT_START;
                  }
                }
                if (check_border(bola, player2)) {
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = MOVE_RIGHT_START;
                  }else {
                    ball_state = MOVE_RIGHT_START;
                  }
                }
              }
            }

            if (scancode == MAKECODE_D) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_move == PLAYER_MOVE_NONE) {
                  if (player2_state_jump != PLAYER_JUMP_NONE) {
                    player2_state_move = PLAYER_MOVE_RIGHT;
                  }
                  else {
                    player2_state_move = PLAYER_MOVE_RIGHT_START;
                  }
                }
                if (check_border(bola, player2)) {
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = MOVE_LEFT_START;
                  }else {
                    ball_state = MOVE_LEFT_START;
                  }
                }
              }
            }

            if (scancode == BREAKCODE_A) {
              if (game_state == MULTIPLAYER) {
                player2_state_move = PLAYER_MOVE_LEFT_END;
              }
            }

            if (scancode == BREAKCODE_D) {
              if (game_state == MULTIPLAYER) {
                player2_state_move = PLAYER_MOVE_RIGHT_END;
              }
            }

            if (scancode == MAKECODE_W) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_jump == PLAYER_JUMP_NONE) {
                  player2_state_jump = START_PLAYER_JUMP;
                }
              }
            }

            if (scancode == MAKECODE_S) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_kick == PLAYER_KICK_NONE) {
                  player2_state_kick = PLAYER_KICK_START;
                }
                kicking2 = true;
              }
            }

            if (scancode == MAKECODE_UP) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (player1_state_jump == PLAYER_JUMP_NONE) {
                  player1_state_jump = START_PLAYER_JUMP;
                }
              }
              else if (game_state == MENU) {
                if (menu_state == HOVER_SINGLEPLAYER) {
                  menu_state = HOVER_EXIT;
                }
                else if (menu_state == HOVER_MULTIPLAYER) {
                  menu_state = HOVER_SINGLEPLAYER;
                }
                else if (menu_state == HOVER_EXIT) {
                  menu_state = HOVER_MULTIPLAYER;
                }
              }
              else if (game_state == PAUSE) {
                if (menu_pause_state == HOVER_RESUME) {
                  menu_pause_state = HOVER_GO_BACK;
                }else if (menu_pause_state == HOVER_GO_BACK) {
                  menu_pause_state = HOVER_RESUME;
                }
              }
              else if (game_state == PAUSE) {
                if (menu_pause_state == HOVER_RESUME) {
                  menu_pause_state = HOVER_GO_BACK;
                }
                else if (menu_pause_state == HOVER_GO_BACK) {
                  menu_pause_state = HOVER_RESUME;
                }
              }
            }

            if (scancode == MAKECODE_DOWN) {
              if (game_state == MENU) {
                if (menu_state == HOVER_SINGLEPLAYER) {
                  menu_state = HOVER_MULTIPLAYER;
                }
                else if (menu_state == HOVER_MULTIPLAYER) {
                  menu_state = HOVER_EXIT;
                }
                else if (menu_state == HOVER_EXIT) {
                  menu_state = HOVER_SINGLEPLAYER;
                }
              }
              else if (game_state == PAUSE) {
                if (menu_pause_state == HOVER_RESUME) {
                  menu_pause_state = HOVER_GO_BACK;
                }
                else if (menu_pause_state == HOVER_GO_BACK) {
                  menu_pause_state = HOVER_RESUME;
                }
              }
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (player1_state_kick == PLAYER_KICK_NONE) {
                  player1_state_kick = PLAYER_KICK_START;
                }
                else {
                  player1_state_kick = AFTER_PLAYER_KICK;
                  player1_state_kick_temporary = PLAYER_KICK_START;
                }
                kicking = true;
              }
            }

            if (scancode == MAKECODE_ENTER) {
              enter_new_state(&game_state, &menu_state, &menu_pause_state, &done);
            }

            if (scancode == BREAKCODE_ESC) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (game_state != STATE_GAME_END && game_state != TIME_LIMIT && bola->stop == false && game_state != GOAL) {
                  game_state = PREPARE_PAUSE;
                }
              }
            }
          }

          if ((msg.m_notify.interrupts & mouse_int_bit) && mouse_active) { 
            // subscribed mouse interrupt
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

          if (msg.m_notify.interrupts & rtc_int_bit) { 
            // subscribed rtc interrupt
            rtc_ih();

            // For debugging: Only works if struct is public
            // rtc_data_t date_time = get_current_time();
            // printf("Today is: %d/%d/%d, %d. Current time is %d:%d:%d\n", date_time.day, date_time.month, date_time.year, date_time.weekday, date_time.hours, date_time.minutes, date_time.seconds);
          }

          if (msg.m_notify.interrupts & uart_int_bit) { 
            // subscribed uart interrupt
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
  if (rtc_unsubscribe_int()) {
    printf("Error while unsubscribing rtc ints!\n");
    return 1;
  }
  if (uart_unsubscribe_int()) {
    printf("Error while unsubscribing uart ints!\n");
    return 1;
  }

  // ======= Destroy objects =======
  destroy_cursor(cursor);
  destroy_button(single);
  destroy_button(multi);
  destroy_button(end);
  destroy_button(resume);
  destroy_button(go_back);

  destroy_ball(bola);
  destroy_player(player1);
  destroy_player(player2);
  destroy_wall(muro);
  destroy_goal(goal);
  destroy_goal(goal_right);
  destroy_scoreboard(scoreboard);
  destroy_timeboard(timeboard);

  // ======= End graphics =======
  if (free_buffers()) {
    printf("Error while freeing buffers!\n");
    return 1;
  }
  if (vg_exit()) {
    printf("Error while returning to text mode!\n");
    return 1;
  }

  return 0;
}

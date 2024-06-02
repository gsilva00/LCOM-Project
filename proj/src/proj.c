#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "game_state.h"

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

#include "objects/ball.h"
#include "objects/button.h"
#include "objects/cursor.h"
#include "objects/goal.h"
#include "objects/scoreboard.h"
#include "objects/timeboard.h"
#include "objects/wall.h"
#include "objects/object_controllers/ball_controller.h"
#include "objects/object_controllers/player_controller.h"
#include "objects/object_controllers/sky_controller.h"

#include "images/objects/baliza-tras-right.xpm"
#include "images/objects/bola.xpm"
#include "images/objects/cursor.xpm"
#include "images/objects/goal_back.xpm"
#include "images/objects/goal_front.xpm"
#include "images/objects/goal_front_right.xpm"
#include "images/objects/goal_card.xpm"
#include "images/objects/scoreboard_90.xpm"
#include "images/objects/timeboard_90.xpm"
#include "images/backgrounds/sky_10am.xpm"
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

// ======= Define XPMs =======
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

// ======= Define Non-Objects =======
static xpm_image_t goal_img;
static xpm_image_t goal_right_img;
static xpm_image_t sky_img;
static xpm_image_t menu_img;
static xpm_image_t pause_menu_img;
static xpm_image_t player1win_menu_img;
static xpm_image_t player2win_menu_img;
static xpm_image_t gamedraw_menu_img;
static xpm_image_t time_reached_menu_img;
static xpm_image_t goal_card_menu_img;

// ======= Define Objects =======
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

void draw_menus(xpm_image_t menu_img, Button *one, Button *two, Button *three, Cursor *cursor, bool cursor_active) {
  draw_xpm(0, 0, menu_img, true);
  if (one != NULL) draw_xpm(one->x, one->y, one->img, true);
  if (two != NULL) draw_xpm(two->x, two->y, two->img, true);
  if (three != NULL) draw_xpm(three->x, three->y, three->img, true);
  if (cursor != NULL && cursor_active) draw_xpm(cursor->x, cursor->y, cursor->img, true);
}

// Done is false when there's more to draw
int draw_game(bool multiplayer, bool done) {
  // Draw scoreboard - clear existing numbers
  if (draw_to_background(scoreboard->x, scoreboard->y, scoreboard->img)) {
    printf("Error while drawing scoreboard!\n");
    return 1;
  }
  // Draw timeboard - clear existing numbers
  if (draw_to_background(timeboard->x, timeboard->y, timeboard->img)) {
    printf("Error while drawing timeboard!\n");
    return 1;
  }

  // Draw scores on the scoreboard
  if (draw_nums_to_background(scoreboard->x + scoreboard->img.width / 5, scoreboard->y + scoreboard->img.height / 3, scoreboard->points1)) {
    printf("Error while drawing scoreboard left score!\n");
    return 1;
  }
  if (draw_nums_to_background(scoreboard->x + scoreboard->img.width / 2 + scoreboard->img.width / 5, scoreboard->y + scoreboard->img.height / 3, scoreboard->points2)) {
    printf("Error while drawing scoreboard right score!\n");
    return 1;
  }

  // Draw time on the timeboard
  int minutes = timeboard->time / 60;
  int seconds = timeboard->time % 60;
  if (draw_nums_to_background(timeboard->x + 15, timeboard->y + timeboard->img.height / 3, minutes / 10)) {
    printf("Error while drawing minutes' 1st digit!");
    return 1;
  }
  if (draw_nums_to_background(timeboard->x + 35, timeboard->y + timeboard->img.height / 3, minutes % 10)) {
    printf("Error while drawing minutes' 2nd digit!\n");
    return 1;
  }

  if (draw_nums_to_background(timeboard->x + 80, timeboard->y + timeboard->img.height / 3, seconds / 10)) {
    printf("Error while drawing seconds' 1st digit!\n");
    return 1;
  }
  if (draw_nums_to_background(timeboard->x + 100, timeboard->y + timeboard->img.height / 3, seconds % 10)) {
    printf("Error while drawing seconds' 2nd digit!\n");
    return 1;
  }


  prep_next_frame();
  // Draw characters from the game
  draw_xpm(bola->x, bola->y, bola->img, false);

  draw_xpm(player1->x, player1->y, player1->img, true);
  if (multiplayer) {
    draw_xpm(player2->x, player2->y, player2->img, true);
  }

  draw_xpm(goal->x - 8, goal->y - 18, goal_img, true);
  draw_xpm(goal_right->x, goal_right->y - 18, goal_right_img, true);
  if (done) end_next_frame();

  return 0;
}


bool is_cursor_touching(Cursor *cursor, Button *button) {
  uint16_t cursor_end = cursor->x + cursor->img.width;
  uint16_t cursor_bottom = cursor->y + cursor->img.height;
  uint16_t button_end = button->x + button->img.width;
  uint16_t button_bottom = button->y + button->img.height;
  return !((cursor_bottom < button->y || button_bottom < cursor->y) || (cursor_end < button->x || button_end < cursor->x));
}
bool check_cursor_position(Cursor *cursor, bool cursor_active, Button *one, Button *two, Button *three, GameState game_state, MenuState *menu_state, MenuPauseState *menu_pause_state) {

  bool is_touching_one = (one != NULL) ? is_cursor_touching(cursor, one) : false;
  bool is_touching_two = (two != NULL) ? is_cursor_touching(cursor, two) : false;
  bool is_touching_three = (three != NULL) ? is_cursor_touching(cursor, three) : false;

  // Make mouse deactivate if the cursor's not active
  if (!cursor_active) return false;

  if (is_touching_one) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_SINGLEPLAYER;
    }
    else if (game_state == PAUSE && menu_pause_state != NULL) {
      *menu_pause_state = HOVER_RESUME;
    }
  }
  if (is_touching_two) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_MULTIPLAYER;
    }
    else if ((game_state == PAUSE || game_state == MULTIPLAYER_END) && menu_pause_state != NULL) {
      *menu_pause_state = HOVER_GO_BACK;
    }
  }
  if (is_touching_three) {
    if (game_state == MENU && menu_state != NULL) {
      *menu_state = HOVER_EXIT;
    }
  }
  return is_touching_one || is_touching_two || is_touching_three;
}


void reset_states(bool multiplayer) {
  if (ball_state != STATE_NONE) {
    ball_state = JUMP_END;
  }
  else {
    ball_state = STATE_NONE;
  }

  if (player1_state_move != PLAYER_MOVE_NONE) {
    player1_state_move = PLAYER_MOVE_END;
    player1_state_jump = PLAYER_JUMP_END;
  }
  else {
    player1_state_move = PLAYER_MOVE_NONE;
    player1_state_jump = PLAYER_JUMP_NONE;
  }

  if (multiplayer) {
    if (player2_state_move != PLAYER_MOVE_NONE) {
      player2_state_move = PLAYER_MOVE_END;
      player2_state_jump = PLAYER_JUMP_END;
    }
    else {
      player2_state_move = PLAYER_MOVE_NONE;
      player2_state_jump = PLAYER_JUMP_NONE;
    }
  }
}


void setup_singleplayer() {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  timeboard->time = 600;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  reset_states(false);
}
void setup_multiplayer() {
  scoreboard->points1 = 0;
  scoreboard->points2 = 0;
  timeboard->time = 300;
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  move_player2_to_center(player2);
  reset_states(true);
}
void restart_game(bool multiplayer) {
  move_ball_to_center(bola);
  move_player1_to_center(player1);
  if (multiplayer) move_player2_to_center(player2);
  reset_states(multiplayer);
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

  // ======= Configuring devices =======
  if (enable_stream_data()) {
    printf("Error while configuring Mouse!\n");
    return 1;
  }
  if (configure_rtc()) {
    printf("Error while configuring RTC!\n");
    return 1;
  }
  // uint8_t sync_tries = 60; // 60 frames of tries to sync (2 seconds)
  if (configure_uart()) {
    printf("Error while configuring UART!\n");
    return 1;
  }

  // ======= Loading non-objects =======
  xpm_load(goal_front_map, XPM_8_8_8, &goal_img);
  xpm_load(goal_right_front_map, XPM_8_8_8, &goal_right_img);
  xpm_load((xpm_map_t) sky_10am_xpm, XPM_8_8_8, &sky_img);
  xpm_load(menu_map, XPM_8_8_8, &menu_img);
  xpm_load(pause_menu_map, XPM_8_8_8, &pause_menu_img);
  xpm_load(player1win_menu_map, XPM_8_8_8, &player1win_menu_img);
  xpm_load(player2win_menu_map, XPM_8_8_8, &player2win_menu_img);
  xpm_load(draw_menu_map, XPM_8_8_8, &gamedraw_menu_img);
  xpm_load(time_reached_menu_map, XPM_8_8_8, &time_reached_menu_img);
  xpm_load(goal_card_menu_map, XPM_8_8_8, &goal_card_menu_img);


  // ======= Loading objects =======
  cursor = create_cursor(cursor_map);
  single = create_button(start_selected_map, 500, 155, true);
  multi = create_button(start_not_selected_map, 500, 219, false);
  end = create_button(end_not_selected_map, 500, 284, false);
  resume = create_button(start_selected_map, 500, 155, true);
  go_back = create_button(end_not_selected_map, 500, 219, false);

  bola = create_ball(bola_map, 400, 490, 62, 32, 10, 50);
  player1 = create_player(player1_map0, 200, 450, 62, 78, 10, 50, 0);
  player2 = create_player(player2_map0, 600, 450, 62, 78, 10, 50, 1);
  muro = create_wall(ground_map);
  goal = create_goal(goal_map, false);
  goal_right = create_goal(goal_right_map, true);
  scoreboard = create_scoreboard(scoreboard_map);
  timeboard = create_timeboard(tb_map);

  // ======= Drawing Start =======
  uint8_t current_hour = get_current_hour();
  if (change_sky_image(current_hour, &sky_img)) {
    printf("Error while loading initial sky image!\n");
    return 1;
  }

  uint16_t background_xx[] = {0, 0, goal->x, goal_right->x};
  uint16_t background_yy[] = {286, 0, goal->y, goal_right->y};
  xpm_image_t background_imgs[] = {muro->img, sky_img, goal->img, goal_right->img};
  if (init_background(background_xx, background_yy, background_imgs, sizeof(background_xx)/sizeof(background_xx[0]))) {
    printf("Error while initializing the third buffer!\n");
    return 1;
  }

  bool cursor_active = true;
  if (game_state == MENU) {
    prep_next_frame();
    draw_menus(menu_img, single, multi, end, cursor, cursor);
    end_next_frame();
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

  bool touching1 = false, touching2 = false, kicking = false, kicking2 = false;
  bool done = false;

  uint8_t mouse_big_packet[3];
  uint8_t counter_byte_packet = 0;
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
                  single->selected = true;
                  multi->selected = false;
                  end->selected = false;
                  single->map = xpm_load(start_selected_map, XPM_8_8_8, &single->img);
                  multi->map = xpm_load(start_not_selected_map, XPM_8_8_8, &multi->img);
                  end->map = xpm_load(end_not_selected_map, XPM_8_8_8, &end->img);
                }
                if (menu_state == HOVER_MULTIPLAYER && multi->selected == false) {
                  single->selected = false;
                  multi->selected = true;
                  end->selected = false;
                  single->map = xpm_load(start_not_selected_map, XPM_8_8_8, &single->img);
                  multi->map = xpm_load(start_selected_map, XPM_8_8_8, &multi->img);
                  end->map = xpm_load(end_not_selected_map, XPM_8_8_8, &end->img);

                }
                else if (menu_state == HOVER_EXIT && end->selected == false) {
                  single->selected = false;
                  multi->selected = false;
                  end->selected = true;
                  single->map = xpm_load(start_not_selected_map, XPM_8_8_8, &single->img);
                  multi->map = xpm_load(start_not_selected_map, XPM_8_8_8, &multi->img);
                  end->map = xpm_load(end_selected_map, XPM_8_8_8, &end->img);
                }
                prep_next_frame();
                draw_menus(menu_img, single, multi, end, cursor, cursor_active);
                check_cursor_position(cursor, cursor_active, single, multi, end, game_state, &menu_state, NULL);
                end_next_frame();
              }
              else if (game_state == PREPARE_SINGLEPLAYER) {
                single->selected = true;
                multi->selected = false;
                end->selected = false;

                setup_singleplayer();
                draw_game(false, true);
                game_state = SINGLEPLAYER;
                cursor_active = false;
              }
              else if (game_state == PREPARE_MULTIPLAYER) {
                // If there's nothing in the queue - other machine didn't send anything, it will remain at 0xFF
                // uint8_t chr = 0xFF;
                // get_queue(get_receive_q(), &chr);

                // if (sync_tries) {
                //   if (chr == 0xFF) {
                //     sync_tries--;

                //     send_char(0x00);
                //   }
                //   else if (chr == ) {
                    
                //   }
                // }
                // else if (sync_tries == -1) { // Connection was established successfully
                  single->selected = false;
                  multi->selected = true;
                  end->selected = false;

                  setup_multiplayer();
                  draw_game(true, true);
                  game_state = MULTIPLAYER;
                  cursor_active = false;
                // }
                // else { // Connection can't be established after all the tries, go back to menu
                //   prep_next_frame();
                //   draw_menus(menu_img, single, multi, end, cursor, cursor_active);
                //   end_next_frame();
                //   game_state = MENU;
                // }
              }
              else if (game_state == PREPARE_PAUSE) {
                game_state = PAUSE;
                cursor_active = true;
                draw_menus(pause_menu_img, resume, go_back, NULL, cursor, cursor_active);
                end_next_frame();
              }
              else if (game_state == PAUSE) {
                if (menu_pause_state == HOVER_RESUME && resume->selected == false) {
                  resume->selected = true;
                  go_back->selected = false;
                  resume->map = xpm_load(start_selected_map, XPM_8_8_8, &resume->img);
                  go_back->map = xpm_load(end_not_selected_map, XPM_8_8_8, &go_back->img);
                }
                if (menu_pause_state == HOVER_GO_BACK && go_back->selected == false) {
                  resume->selected = false;
                  go_back->selected = true;
                  resume->map = xpm_load(start_not_selected_map, XPM_8_8_8, &resume->img);
                  go_back->map = xpm_load(end_selected_map, XPM_8_8_8, &go_back->img);
                }
                draw_game(multi->selected, false);
                draw_menus(pause_menu_img, resume, go_back, NULL, cursor, cursor_active);
                check_cursor_position(cursor, cursor_active, single, multi, NULL, game_state, NULL, &menu_pause_state);
                end_next_frame();
              }
              else if (game_state == PAUSE_TO_PLAY) {
                resume->selected = true;
                go_back->selected = false;

                draw_game(multi->selected, true);

                if (player1->xspeed != 0) {
                  player1_state_move = player1->xspeed < 0 ? AFTER_PLAYER_MOVE_LEFT : AFTER_PLAYER_MOVE_RIGHT;
                }
                if (player2->xspeed != 0) {
                  player2_state_move = player2->xspeed < 0 ? AFTER_PLAYER_MOVE_LEFT : AFTER_PLAYER_MOVE_RIGHT;
                }
                game_state = multi->selected ? MULTIPLAYER : SINGLEPLAYER;
                cursor_active = false;
              }
              else if (game_state == PAUSE_TO_MENU) {
                resume->selected = false;
                go_back->selected = true;

                cursor_active = true;

                prep_next_frame();
                draw_menus(menu_img, single, multi, end, cursor, cursor_active);
                end_next_frame();
                game_state = MENU;
              }
              else if (game_state == MULTIPLAYER_END) {
                if (menu_pause_state == HOVER_GO_BACK && go_back->selected == false) {
                  go_back->selected = true;
                  go_back->map = xpm_load(end_selected_map, XPM_8_8_8, &go_back->img);
                }

                cursor_active = true;

                draw_game(multi->selected, false);

                if (scoreboard->points1 > scoreboard->points2) {
                  draw_menus(player1win_menu_img, NULL, go_back, NULL, cursor, cursor_active);
                }
                else if (scoreboard->points1 < scoreboard->points2) {
                  draw_menus(player2win_menu_img, NULL, go_back, NULL, cursor, cursor_active);
                }
                else {
                  draw_menus(gamedraw_menu_img, NULL, go_back, NULL, cursor, cursor_active);
                }
                check_cursor_position(cursor, cursor_active, NULL, go_back, NULL, game_state, NULL, &menu_pause_state);
                end_next_frame();
              }
              else if (game_state == TIME_LIMIT) {
                go_back->selected = true;
                go_back->map = xpm_load(end_selected_map, XPM_8_8_8, &go_back->img);

                cursor_active = true;

                draw_game(multi->selected, false);
                draw_menus(time_reached_menu_img, NULL, go_back, NULL, cursor, cursor_active);
                end_next_frame();
              }
              else if (game_state == GOAL) {
                cursor_active = false;

                draw_game(multi->selected, true);
                draw_menus(goal_card_menu_img, NULL, NULL, NULL, NULL, cursor_active);
                end_next_frame();
                sleep(1);

                restart_game(multi->selected);
                draw_game(multi->selected, true);

                game_state = multi->selected ? MULTIPLAYER : SINGLEPLAYER;

                prep_next_frame();
                draw_xpm(bola->x, bola->y, bola->img, false);
                draw_xpm(player1->x, player1->y, player1->img, true);
                if (multi->selected) {
                  draw_xpm(player2->x, player2->y, player2->img, true);
                }
                end_next_frame();
              }
              else if (game_state == SINGLEPLAYER) {
                if (ball_state != STATE_NONE) {
                  prep_next_frame();
                }
                if (player1_state_move != PLAYER_MOVE_NONE || player2_state_move != PLAYER_MOVE_NONE) {
                  prep_next_frame();
                }
                if (player_border_collision(player1)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = PLAYER_MOVE_END;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                if (ball_player_collision(bola, player1) && !touching1) {
                  if (player1->x + player1->width/2 > bola->x + bola->width/2) {
                    if (ball_state != START_JUMP_LEFT && ball_state != START_JUMP_RIGHT && ball_state != JUMP_LEFT && ball_state != JUMP_RIGHT && ball_state != BEFORE_JUMP_LEFT && ball_state != BEFORE_JUMP_RIGHT && ball_state != AFTER_JUMP_LEFT && ball_state != AFTER_JUMP_RIGHT) {
                      if (ball_state != STATE_NONE) {
                        ball_state = JUMP_END;
                        ball_state_temporary = MOVE_LEFT_START;
                      } else {
                        ball_state = MOVE_LEFT_START;
                      }
                    }
                  } else {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    } else {
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if (!ball_player_collision(bola, player1) && touching1) {
                  ball_state = AFTER_MOVE;
                  touching1 = false;
                }
                if (check_kicking_player(bola, player1) && kicking) {
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
                move_ball(bola, &ball_state, &ball_state_temporary);
                ball_goal_collision(bola, goal, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right, scoreboard, &ball_state);
                decrease_time(timeboard);
                draw_game(false, true);
                if (timeboard->time == 0) {
                  game_state = TIME_LIMIT;
                }
                else if (scoreboard->points1 == 9 || scoreboard->points2 == 9) {
                  game_state = MULTIPLAYER_END;
                }
                if (bola->stop) {
                  game_state = GOAL;
                }
              }
              else if (game_state == MULTIPLAYER) {
                if (ball_state != STATE_NONE) {
                  prep_next_frame();
                }
                if (player1_state_move != PLAYER_MOVE_NONE || player2_state_move != PLAYER_MOVE_NONE) {
                  prep_next_frame();
                }
                if (player_border_collision(player1)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = PLAYER_MOVE_END;
                  }
                }
                if (player_border_collision(player2)) {
                  if (player2_state_move != PLAYER_MOVE_NONE) {
                    player2_state_move = PLAYER_MOVE_END;
                  }
                }
                if (player_player_collision(player1, player2)) {
                  if (player1_state_move != PLAYER_MOVE_NONE) {
                    player1_state_move = PLAYER_MOVE_END;
                  }
                  if (player2_state_move != PLAYER_MOVE_NONE) {
                    player2_state_move = PLAYER_MOVE_END;
                  }
                }
                move_player(player1, &player1_state_move, &player1_state_move_temporary, &player1_state_jump, &player1_state_jump_temporary, &player1_state_kick, &player1_state_kick_temporary);
                move_player(player2, &player2_state_move, &player2_state_move_temporary, &player2_state_jump, &player2_state_jump_temporary, &player2_state_kick, &player2_state_kick_temporary);
                if (ball_player_collision(bola, player1) && !touching1) {
                  printf("OOOO");
                  if (player1->x + player1->width/2 > bola->x + bola->width/2) {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_LEFT_START;
                    } else {
                      ball_state = MOVE_LEFT_START;
                    }
                  } else {
                    if (ball_state != STATE_NONE) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    } else {
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching1 = true;
                }
                else if (!ball_player_collision(bola, player1) && touching1) {
                  ball_state = AFTER_MOVE;
                  ball_state_temporary = STATE_NONE;
                  touching1 = false;
                }

                printf("ball_state %d\n", ball_state);
                if (ball_player_collision(bola, player2)) {
                  printf("IIII");
                  
                  if (player2->x + player2->width/2 > bola->x + bola->width/2) {
                    if (ball_state != STATE_NONE  && ball_state != MOVE_LEFT_START && ball_state != MOVE_LEFT) {
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_LEFT_START;
                    } else if(ball_state != MOVE_LEFT){
                      ball_state = MOVE_LEFT_START;
                    }
                  } else {
                    if (ball_state != STATE_NONE && ball_state != MOVE_RIGHT_START && ball_state != MOVE_RIGHT) {
                      printf("end");
                      ball_state = JUMP_END;
                      ball_state_temporary = MOVE_RIGHT_START;
                    } else if(ball_state != MOVE_RIGHT){
                      printf("right");
                      ball_state = MOVE_RIGHT_START;
                    }
                  }
                  touching2 = true;
                }
                else if (!ball_player_collision(bola, player2) && touching2) {
                  printf("after");
                  ball_state = AFTER_MOVE;
                  ball_state_temporary = STATE_NONE;
                  touching2 = false;
                }

                if (check_kicking_player(bola, player1) && kicking) {
                  printf("kick\n");
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = START_JUMP_RIGHT;
                  } else {
                    ball_state = START_JUMP_RIGHT;
                  }
                  kicking = false;
                }
                if (check_kicking_player(bola, player2) && kicking2) {
                  printf("kick\n");
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = START_JUMP_LEFT;
                  } else {
                    ball_state = START_JUMP_LEFT;
                  }
                  kicking2 = false;
                }
                move_ball(bola, &ball_state, &ball_state_temporary);
                ball_goal_collision(bola, goal, scoreboard, &ball_state);
                ball_goal_collision(bola, goal_right, scoreboard, &ball_state);
                decrease_time(timeboard);
                draw_game(true, true);
                if (scoreboard->points1 == 5 || scoreboard->points2 == 5 || timeboard->time == 0) {
                  game_state = MULTIPLAYER_END;
                }
                if (bola->stop) {
                  game_state = GOAL;
                }
              }
            }
          }
          if (msg.m_notify.interrupts & keyboard_int_bit) { 
            kbc_ih();

            uint8_t scancode = get_scancode();

            if (scancode == SCANCODE_E0) {
              continue;
            }

            /* For debugging
            bool make = !(scancode & BREAKCODE);
            if (kbd_print_scancode(make, 1, &scancode)) {
              return 1;
            }
            */


            // PLAYER 1 CONTROLS
            cursor_active = false;
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
                } else if (menu_pause_state == HOVER_GO_BACK) {
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
                if(check_kicking_player(bola, player1)){
                  kicking = true;
                }
              }
            }

            if (scancode == MAKECODE_RIGHT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (player1_state_move == PLAYER_MOVE_NONE) {
                  if (player1_state_jump != PLAYER_JUMP_NONE) {
                    player1_state_move = PLAYER_MOVE_RIGHT;
                  } else {
                    player1_state_move = PLAYER_MOVE_RIGHT_START;
                  }
                }
              }
            }

            if (scancode == BREAKCODE_LEFT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                player1_state_move = PLAYER_MOVE_END;
              }
            }

            if (scancode == BREAKCODE_RIGHT) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                player1_state_move = PLAYER_MOVE_END;
              }
            }

            // PLAYER 2 CONTROLS
            if (scancode == MAKECODE_W) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_jump == PLAYER_JUMP_NONE) {
                  player2_state_jump = START_PLAYER_JUMP;
                }
              }
            }
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
                if (ball_player_collision(bola, player2)) {
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = MOVE_RIGHT_START;
                  } else {
                    ball_state = MOVE_RIGHT_START;
                  }
                }
              }
            }
            if (scancode == MAKECODE_S) {
              if (game_state == MULTIPLAYER) {
                if (player2_state_kick == PLAYER_KICK_NONE) {
                  player2_state_kick = PLAYER_KICK_START;
                }
                else {
                  player2_state_kick = AFTER_PLAYER_KICK;
                  player2_state_kick_temporary = PLAYER_KICK_START;
                }
                if(check_kicking_player(bola, player2)){
                  kicking2 = true;
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
                if (ball_player_collision(bola, player2)) {
                  if (ball_state != STATE_NONE) {
                    ball_state = JUMP_END;
                    ball_state_temporary = MOVE_LEFT_START;
                  } else {
                    ball_state = MOVE_LEFT_START;
                  }
                }
              }
            }

            if (scancode == BREAKCODE_A) {
              if (game_state == MULTIPLAYER) {
                player2_state_move = PLAYER_MOVE_END;
              }
            }

            if (scancode == BREAKCODE_D) {
              if (game_state == MULTIPLAYER) {
                player2_state_move = PLAYER_MOVE_END;
              }
            }

            if (scancode == MAKECODE_ENTER) {
              enter_new_state(&game_state, &menu_state, &menu_pause_state, &done, &cursor_active);
            }

            if (scancode == BREAKCODE_ESC) {
              if (game_state == SINGLEPLAYER || game_state == MULTIPLAYER) {
                if (game_state != MULTIPLAYER_END && game_state != TIME_LIMIT && bola->stop == false && game_state != GOAL) {
                  game_state = PREPARE_PAUSE;
                }
              }
            }
          }

          if (msg.m_notify.interrupts & mouse_int_bit) { 
            mouse_ih();

            uint8_t packetByte = get_packetByte();
            if (counter_byte_packet == 0 && !(packetByte & BIT(3))) {
              printf("Error: This packet's first byte has bit 3 at 0!\n");
              continue;
            }

            mouse_big_packet[counter_byte_packet] = packetByte;
            counter_byte_packet++;

            if (counter_byte_packet == 3) {
              cursor_active = true;

              bool clicked_lb = mouse_big_packet[0] & PS2_LB;

              bool exists_x_ovf = mouse_big_packet[0] & PS2_XOVER;
              bool exists_y_ovf = mouse_big_packet[0] & PS2_YOVER;

              if (!exists_x_ovf && !exists_y_ovf) {
                int16_t delta_x;
                int16_t delta_y;

                if (mouse_big_packet[0] & PS2_XDELTA) {
                  uint16_t x = 0xFF00 | mouse_big_packet[1];
                  delta_x = ((~x) + 1);
                }
                else {
                  delta_x = mouse_big_packet[1];
                }

                if (mouse_big_packet[0] & PS2_YDELTA) {
                  uint16_t y = 0xFF00 | mouse_big_packet[2];
                  delta_y = -((~y) + 1);
                }
                else {
                  delta_y = mouse_big_packet[2];
                }

                set_coordinates(cursor, delta_x % 800, delta_y % 600, mouse_big_packet[0] & PS2_XDELTA, !(mouse_big_packet[0] & PS2_YDELTA));

                if (check_cursor_position(cursor, cursor_active, single, multi, end, game_state, &menu_state, &menu_pause_state) && clicked_lb) {
                  enter_new_state(&game_state, &menu_state, &menu_pause_state, &done, &cursor_active);
                }
              }

              counter_byte_packet = 0;
            }
          }

          if (msg.m_notify.interrupts & rtc_int_bit) { 
            rtc_ih();

            // Get the hour that was just updated
            uint8_t read_hour = get_current_hour();
            if (current_hour != read_hour) { // If they're different, update the sky
              current_hour = read_hour;
              change_sky_image(current_hour, &sky_img);
            }

            // Redraw the buffer with updated sky
            uint16_t background_xx[] = {0, 0, goal->x, goal_right->x};
            uint16_t background_yy[] = {286, 0, goal->y, goal_right->y};
            xpm_image_t background_imgs[] = {muro->img, sky_img, goal->img, goal_right->img};
            if (init_background(background_xx, background_yy, background_imgs, sizeof(background_xx)/sizeof(background_xx[0]))) {
              printf("Error while initializing the third buffer!\n");
              return 1;
            }
          }

          if (msg.m_notify.interrupts & uart_int_bit) { 
            uart_ih();
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

  // ======= Unsubscribe interrupts =======
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

  // ======= Reset devices =======
  if (disable_stream_data()) {
    printf("Error while resetting mouse!\n");
    return 1;
  }
  if (reset_uart()) {
    printf("Error while resetting UART!\n");
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

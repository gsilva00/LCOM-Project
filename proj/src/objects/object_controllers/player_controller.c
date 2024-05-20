#include "player_controller.h"

uint8_t time_passed_player_y = 0;
uint8_t time_passed_player_x = 0;

void initialize_player_values(player *player) {
  player_y_original = player->y;
  player_y = player->y;
  player_x = player->x;
  player_yspeed = player->yspeed;
  player_xspeed = player->xspeed;
  chuta = false;
  printf("player_y_original: %d\n", player_y_original);
}

void prepare_for_player_jump(player *player) {
  chuta = true;
  time_passed_player_y = 1;
  time_passed_player_x = 1;
  bounce_offset = 0;
}

void update_player_position_after_jump(player *player) {
  player->yspeed = player->yspeed * 0.65;
  player_y = player->y;
  player_x = player->x;
}

void check_player_border(player *player) {
  if (player->x >= 800) {
    player->x -= 800;
  }
  if (player->x <= 0) {
    player->x += 800;
  }
}

void change_player_y(player *player) {
  player->y = player_y - ((player->yspeed * time_passed_player_y) - 5 * (time_passed_player_y * time_passed_player_y));

  if (player->y >= 200) {
    player->y = 200;
    chuta = false;
  }
}

void handle_player_jump(player *player, PlayerStateJump *player_state_jump, int direction) {
      check_player_border(player);

      if ((player->x >= BARRIER_START) && (player->x <= BARRIER_END)) { // ou qualquer outra barreira
        player->x = direction == 1 ? BARRIER_START : BARRIER_END;
        time_passed_player_x = -1;
      }

      change_player_y(player);
      draw_xpm(player->x, player->y, player->img);
      time_passed_player_y++;
      time_passed_player_x++;

      *player_state_jump = STATE_PLAYER_JUMP_END;
}

void(move_player)(player *player, PlayerStateMove *player_state_move, PlayerStateMove *player_state_move_temporary, PlayerStateJump *player_state_jump, PlayerStateJump *player_state_jump_temporary) {
  switch (*player_state_move) {
    case STATE_PLAYER_MOVE_LEFT_START:
      initialize_player_values(player);
      *player_state_move = STATE_PLAYER_MOVE_LEFT;
      break;
    case STATE_PLAYER_MOVE_RIGHT_START:
      initialize_player_values(player);
      *player_state_move = STATE_PLAYER_MOVE_RIGHT;
      break;
    case STATE_PLAYER_MOVE_LEFT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x -= player->xspeed;
        check_player_border(player);
        draw_xpm(player->x, player->y, player->img);
      }
      break;

    case STATE_PLAYER_MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x += player->xspeed;
        check_player_border(player);
        draw_xpm(player->x, player->y, player->img);
      }
      break;
    case STATE_AFTER_PLAYER_MOVE_LEFT:
      if(player->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }else{
        *player_state_move = STATE_PLAYER_MOVE_END;
      }
      break;
    case STATE_AFTER_PLAYER_MOVE_RIGHT:
    if(player->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }else{
        *player_state_move = STATE_PLAYER_MOVE_END;
      }
      break;
    case STATE_PLAYER_MOVE_END:
      player->xspeed = player_xspeed;
      *player_state_move = STATE_PLAYER_MOVE_NONE;
      break;
    case STATE_PLAYER_MOVE_NONE:
      if (*player_state_move_temporary != STATE_PLAYER_MOVE_NONE) {
        *player_state_move = *player_state_move_temporary;
        *player_state_move_temporary = STATE_PLAYER_MOVE_NONE;
      }
      break;
    default:
      break;
  }

  switch(*player_state_jump){
    case STATE_START_PLAYER_JUMP:
      initialize_player_values(player);
      *player_state_jump = STATE_PLAYER_JUMP;
      break;
    case STATE_PLAYER_JUMP:
      handle_player_jump(player, player_state_jump, -1);
      break;
    case STATE_PLAYER_JUMP_END:
      player->yspeed = player_yspeed;
      player->y = player_y_original;
      *player_state_jump = STATE_PLAYER_JUMP_NONE;
      break;
    case STATE_PLAYER_JUMP_NONE:
      if (*player_state_jump_temporary != STATE_PLAYER_JUMP_NONE) {
        *player_state_jump = *player_state_jump_temporary;
        *player_state_jump_temporary = STATE_PLAYER_JUMP_NONE;
      }
      break;
    case STATE_BEFORE_PLAYER_JUMP:
      prepare_for_player_jump(player);
      *player_state_jump = player->yspeed > 0 ? STATE_PLAYER_JUMP : STATE_PLAYER_JUMP_END;
      break;
    case STATE_AFTER_PLAYER_JUMP:
      update_player_position_after_jump(player);
      *player_state_jump = STATE_BEFORE_PLAYER_JUMP;
      break;
    default:
      break;
  }
}

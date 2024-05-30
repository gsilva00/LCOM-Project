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
}

void check_player_border(player *player) {

  if ((player->x >= BARRIER_START) && (player->x <= BARRIER_END)) {
    player->x = BARRIER_END;
  }

  if ((player->x + player->img.width >= BARRIER_START_1) && (player->x <= BARRIER_END_1)) {
    player->x = BARRIER_START_1 - player->img.width;
  }

  /*if (player->x >= 800) {
    player->x -= 800;
  }
  if (player->x <= 0) {
    player->x += 800;
  }*/
}

void change_player_y(player *player) {
  player->y = player_y - ((player->yspeed * time_passed_player_y) - 5 * (time_passed_player_y * time_passed_player_y));
}

void handle_player_jump(player *player, PlayerStateJump *player_state_jump) {
  if (get_timer_intCounter() % 2 == 0) {
    check_player_border(player);

    change_player_y(player);
    time_passed_player_y++;
    time_passed_player_x++;

    if(player->y >= player_y_original){
      player->y = player_y_original;
      chuta = false;
      *player_state_jump = STATE_PLAYER_JUMP_END;
    }
  }
}

int restart_palyer_movement(player *player){
  player->xspeed = player_xspeed;
  player->yspeed = player_yspeed;
  return 0;
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
      }
      break;

    case STATE_PLAYER_MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x += player->xspeed;
        check_player_border(player);
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
      restart_palyer_movement(player);
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
      *player_state_jump = STATE_BEFORE_PLAYER_JUMP;
      break;
    case STATE_BEFORE_PLAYER_JUMP:
      prepare_for_player_jump(player);
      *player_state_jump = player->yspeed > 0 ? STATE_PLAYER_JUMP : STATE_PLAYER_JUMP_END;
      break;
    case STATE_PLAYER_JUMP:
      handle_player_jump(player, player_state_jump);
      break;
    case STATE_PLAYER_JUMP_END:
      restart_palyer_movement(player);
      *player_state_jump = STATE_PLAYER_JUMP_NONE;
      break;
    case STATE_PLAYER_JUMP_NONE:
      if (*player_state_jump_temporary != STATE_PLAYER_JUMP_NONE) {
        *player_state_jump = *player_state_jump_temporary;
        *player_state_jump_temporary = STATE_PLAYER_JUMP_NONE;
      }
      break;
    default:
      break;
  }
}

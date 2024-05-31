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

bool check_player_border(player *player){
  if (player->x + player->width >= 800) {
    player->x--;
    return true;
  }
  if (player->x <= 0) {
    player->x++;
    return true;
  }
  return false;
}

bool check_multiplayer_border(player *player1, player *player2) {
  if ((player1->x <= player2->x + player2->width + 5) && (player1->x + player1->width + 5 >= player2->x)){
    if ((player1->y <= player2->y + player2->height + 5) && (player1->y + player1->height + 5 >= player2->y)){
      if(player1->x < player2->x){
        player1->x--;
        player2->x++;
      }else{
        player1->x++;
        player2->x--;
      }
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}

void change_player_y(player *player) {
  player->y = player_y - ((player->yspeed * time_passed_player_y) - 5 * (time_passed_player_y * time_passed_player_y));
}

void handle_player_jump(player *player, PlayerStateJump *player_state_jump) {
  if (get_timer_intCounter() % 2 == 0) {

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

int restart_player_movement(player *player){
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
      }
      break;

    case STATE_PLAYER_MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        player->x += player->xspeed;
      }
      break;
    case STATE_AFTER_PLAYER_MOVE_LEFT:
      if(player->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }else{
        *player_state_move = STATE_PLAYER_MOVE_LEFT_END;
      }
      break;
    case STATE_AFTER_PLAYER_MOVE_RIGHT:
    if(player->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }else{
        *player_state_move = STATE_PLAYER_MOVE_RIGHT_END;
      }
      break;
    case STATE_PLAYER_MOVE_LEFT_END:
      restart_player_movement(player);
      *player_state_move = STATE_PLAYER_MOVE_NONE;
      break;
    case STATE_PLAYER_MOVE_RIGHT_END:
      restart_player_movement(player);
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
      restart_player_movement(player);
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

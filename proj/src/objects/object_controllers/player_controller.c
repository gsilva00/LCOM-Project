#include "player_controller.h"

uint8_t time_passed_y = 0;
uint8_t time_passed_x = 0;

void initialize_ball_values(player *player) {
  player_y_original = player->y;
  player_y = player->y;
  player_x = player->x;
  player_yspeed = player->yspeed;
  player_xspeed = player->xspeed;
  chuta = false;
}

void prepare_for_jump(player *player) {
  chuta = true;
  time_passed_y = 1;
  time_passed_x = 1;
  bounce_offset = 0;
}

void update_ball_position_after_jump(player *player) {
  player->yspeed = player->yspeed * 0.65;
  player_y = player->y;
  player_x = player->x;
}

void check_border(player *player) {
  if (player->x >= 800) {
    player->x -= 800;
  }
  if (player->x <= 0) {
    player->x += 800;
  }
}

void change_y(player *player) {
  player->y = player_y - ((player->yspeed * time_passed_y) - 5 * (time_passed_y * time_passed_y));

  if (player->y >= 200) {
    player->y = 200;
    chuta = false;
  }
}

void handle_jump(player *player, PlayerState *player_state, int direction) {
  if (get_timer_intCounter() % 2 == 0) {
    if (chuta) {
      player->x = player_x + direction * (player->xspeed * time_passed_x) + bounce_offset;

      check_border(player);

      if ((player->x >= BARRIER_START) && (player->x <= BARRIER_END)) { // ou qualquer outra barreira
        player->x = direction == 1 ? BARRIER_START : BARRIER_END;
        bounce_offset = BOUNCE_OFFSET;
        player->xspeed = -player->xspeed * BOUNCE_SPEED_REDUCTION_FACTOR;
        time_passed_x = -1;
      }

      change_y(player);
      //draw_xpm(player->x, player->y, player->img);
      time_passed_y++;
      time_passed_x++;
    }
    else {
      *player_state = direction == 1 ? STATE_AFTER_JUMP_RIGHT : STATE_AFTER_JUMP_LEFT;
    }
  }
}

void(move_player)(player *player, PlayerState *player_state, PlayerState *player_state_temporary) {
  switch (*player_state) {
    case STATE_START_JUMP_LEFT:
      initialize_player_values(player);
      *player_state = STATE_BEFORE_JUMP_LEFT;
      break;
    case STATE_BEFORE_JUMP_LEFT:
      prepare_for_jump(player);
      *player_state = player->yspeed > 0 ? STATE_JUMP_LEFT : STATE_JUMP_END;
      break;
    case STATE_JUMP_LEFT:
      handle_jump(player, player_state, -1);
      break;
    case STATE_AFTER_JUMP_LEFT:
      update_player_position_after_jump(player);
      *player_state = STATE_BEFORE_JUMP_LEFT;
      break;
    case STATE_JUMP_END:
      player->yspeed = player_yspeed;
      player->y = player_y_original;
      player->xspeed = player_xspeed;
      *player_state = STATE_NONE;
      break;
    case STATE_NONE:
      if (*player_state_temporary != STATE_NONE) {
        *player_state = *player_state_temporary;
        *player_state_temporary = STATE_NONE;
      }
      break;

    case STATE_START_JUMP_RIGHT:
      initialize_player_values(player);
      *player_state = STATE_BEFORE_JUMP_RIGHT;
      break;
    case STATE_BEFORE_JUMP_RIGHT:
      prepare_for_jump(player);
      if (player->yspeed > 0) {
        *player_state = STATE_JUMP_RIGHT;
      }
      else {
        *player_state = STATE_JUMP_END;
      }
      break;
    case STATE_JUMP_RIGHT:
      handle_jump(player, player_state, 1);
      break;
    case STATE_AFTER_JUMP_RIGHT:
      update_player_position_after_jump(player);
      *player_state = STATE_BEFORE_JUMP_RIGHT;
      break;

    case STATE_MOVE_LEFT_START:
      initialize_player_values(player);
      *player_state = STATE_MOVE_LEFT;
      break;
    case STATE_MOVE_RIGHT_START:
      initialize_player_values(player);
      *player_state = STATE_MOVE_RIGHT;
      break;

    case STATE_MOVE_LEFT:
      if (player->xspeed != 0) {
        if (get_timer_intCounter() % 2 == 0) {
          player->x -= player->xspeed;
          check_border(player);
          draw_xpm(player->x, player->y, player->img);
        }
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }
      else {
        *player_state = STATE_JUMP_END;
      }
      break;

    case STATE_MOVE_RIGHT:
      if (player->xspeed != 0) {
        if (get_timer_intCounter() % 2 == 0) {
          player->x += player->xspeed;
          check_border(player);
          draw_xpm(player->x, player->y, player->img);
        }
        if (get_timer_intCounter() % 30 == 0) {
          player->xspeed = player->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", player->xspeed);
        }
      }
      else {
        *player_state = STATE_JUMP_END;
      }
      break;
    default:
      break;
  }
}
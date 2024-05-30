#include "ball_controller.h"

uint8_t time_passed_y = 0;
uint8_t time_passed_x = 0;

void initialize_ball_values(ball *bola) {
  bola_y_original = bola->y;
  bola_y = bola->y;
  bola_x = bola->x;
  bola_yspeed = bola->yspeed;
  bola_xspeed = bola->xspeed;
  chuta = false;
}

void prepare_for_jump(ball *bola) {
  chuta = true;
  time_passed_y = 1;
  time_passed_x = 1;
  bounce_offset = 0;
}

void update_ball_position_after_jump(ball *bola) {
  bola->yspeed = bola->yspeed * 0.65;
  bola_y = bola->y;
  bola_x = bola->x;
}

bool check_border(ball *bola, player *pl) {
  if((bola->x <= pl->x + pl->width + 1) && (bola->x + bola->width + 1 >= pl->x)){
    if((bola->y <= pl->y + pl->height + 1) && (bola->y + bola->height + 1 >= pl->y)){
      return true;
    }else{
      return false;
    }
  }else{
    return true;
  }
}

void change_y(ball *bola) {
  bola->y = bola_y - ((bola->yspeed * time_passed_y) - 5 * (time_passed_y * time_passed_y));

  if (bola->y >= 200) {
    bola->y = 200;
    chuta = false;
  }
}

void handle_jump(ball *bola, BallState *ball_state, int direction) {
  if (get_timer_intCounter() % 2 == 0) {
    if (chuta) {
      bola->x = bola_x + direction * (bola->xspeed * time_passed_x) + bounce_offset;

      if ((bola->x >= BARRIER_START) && (bola->x <= BARRIER_END)) { // ou qualquer outra barreira
        bola->x = direction == 1 ? BARRIER_START : BARRIER_END;
        bounce_offset = BOUNCE_OFFSET;
        bola->xspeed = -bola->xspeed * BOUNCE_SPEED_REDUCTION_FACTOR;
        time_passed_x = -1;
      }

      change_y(bola);

      //draw_xpm(bola->x, bola->y, bola->img);

      time_passed_y++;
      time_passed_x++;
    }
    else {
      *ball_state = direction == 1 ? STATE_AFTER_JUMP_RIGHT : STATE_AFTER_JUMP_LEFT;
    }
  }
}

void(move_ball)(ball *bola, BallState *ball_state, BallState *ball_state_temporary) {
  switch (*ball_state) {
    case STATE_START_JUMP_LEFT:
      initialize_ball_values(bola);
      *ball_state = STATE_BEFORE_JUMP_LEFT;
      break;
    case STATE_BEFORE_JUMP_LEFT:
      prepare_for_jump(bola);
      *ball_state = bola->yspeed > 0 ? STATE_JUMP_LEFT : STATE_JUMP_END;
      break;
    case STATE_JUMP_LEFT:
      handle_jump(bola, ball_state, -1);
      break;
    case STATE_AFTER_JUMP_LEFT:
      update_ball_position_after_jump(bola);
      *ball_state = STATE_BEFORE_JUMP_LEFT;
      break;
    case STATE_JUMP_END:
      bola->yspeed = bola_yspeed;
      bola->y = bola_y_original;
      bola->xspeed = bola_xspeed;
      *ball_state = STATE_NONE;
      break;
    case STATE_NONE:
      if (*ball_state_temporary != STATE_NONE) {
        *ball_state = *ball_state_temporary;
        *ball_state_temporary = STATE_NONE;
      }
      break;

    case STATE_START_JUMP_RIGHT:
      initialize_ball_values(bola);
      *ball_state = STATE_BEFORE_JUMP_RIGHT;
      break;
    case STATE_BEFORE_JUMP_RIGHT:
      prepare_for_jump(bola);
      if (bola->yspeed > 0) {
        *ball_state = STATE_JUMP_RIGHT;
      }
      else {
        *ball_state = STATE_JUMP_END;
      }
      break;
    case STATE_JUMP_RIGHT:
      handle_jump(bola, ball_state, 1);
      break;
    case STATE_AFTER_JUMP_RIGHT:
      update_ball_position_after_jump(bola);
      *ball_state = STATE_BEFORE_JUMP_RIGHT;
      break;

    case STATE_MOVE_LEFT_START:
      initialize_ball_values(bola);
      *ball_state = STATE_MOVE_LEFT;
      break;
    case STATE_MOVE_RIGHT_START:
      initialize_ball_values(bola);
      *ball_state = STATE_MOVE_RIGHT;
      break;

    case STATE_MOVE_LEFT:
      if (get_timer_intCounter() % 2 == 0) {
        bola->x -= bola->xspeed;
        //draw_xpm(bola->x, bola->y, bola->img);
        
      }
      break;

    case STATE_MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        bola->x += bola->xspeed;
        //draw_xpm(bola->x, bola->y, bola->img);
      }
      break;
    case STATE_AFTER_MOVE:
      if (bola->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          bola->xspeed = bola->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", bola->xspeed);
        }
      }else {
        *ball_state = STATE_JUMP_END;
      }
    default:
      break;
  }
}

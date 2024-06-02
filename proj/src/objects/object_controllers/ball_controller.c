#include "ball_controller.h"


// Time variables to simulate acceleration
static uint8_t time_passed_y = 0;
static uint8_t time_passed_x = 0;

static int bola_y_original;
static int bola_y;
static int bola_x;
static int bola_yspeed;
static int bola_xspeed;
static int bounce_offset;
static bool chuta;


void initialize_ball_values(Ball *bola) {
  bola_y_original = bola->y;
  bola_y = bola->y;
  bola_x = bola->x;
  bola_yspeed = bola->yspeed;
  bola_xspeed = bola->xspeed;
  chuta = false;
}

void prepare_for_jump(Ball *bola) {
  chuta = true;
  time_passed_y = 1;
  time_passed_x = 1;
  bounce_offset = 0;
}

void update_ball_position_after_jump(Ball *bola) {
  bola->yspeed = bola->yspeed * 0.65;
  bola_y = bola->y;
  bola_x = bola->x;
}

bool check_kicking_player(Ball *bola, Player *pl) {
  if (bola == NULL || pl == NULL) return false;
  if (pl->orientation == 0) { // Left player
    if ((bola->x <= pl->x + pl->width + 10) && (bola->x > pl->x + pl->width)) {
      if ((bola->y >= pl->y + pl->height/4) && (bola->y + 5 <= pl->y + pl->height)) {
        return true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
  else { // Right player
    if ((bola->x + bola->width >= pl->x - 10) && (bola->x + bola->width < pl->x)) {
      if ((bola->y >= pl->y + pl->height/4) && (bola->y + 5 <= pl->y + pl->height)) {
        return true;
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
}

bool ball_player_collision(Ball *bola, Player *pl) {
  uint16_t bola_end = bola->x + bola->img.width;
  uint16_t bola_bottom = bola->y + bola->img.height;
  uint16_t pl_end = pl->x + pl->img.width;
  uint16_t pl_bottom = pl->y + pl->img.height;
  return !((bola_bottom < pl->y || pl_bottom < bola->y) || (bola_end < pl->x || pl_end < bola->x));
  /*if ((bola->x <= pl->x + pl->width + 5) && (bola->x + bola->width + 5 >= pl->x)) {
    return (bola->y <= pl->y + pl->height + 5) && (bola->y + bola->height + 5 >= pl->y);
  }
  else {
    return false;
  }*/
}

void change_y(Ball *bola) {
  bola->y = bola_y - ((bola->yspeed * time_passed_y) - 5 * (time_passed_y * time_passed_y));

  if (bola->y >= bola_y_original) {
    bola->y = bola_y_original;
    chuta = false;
  }
}

void handle_jump(Ball *bola, BallState *ball_state, int direction) {
  if (get_timer_intCounter() % 2 == 0) {
    if (chuta) {
      bola->x = bola_x + direction * (bola->xspeed * time_passed_x) + bounce_offset;

      if ((bola->x + bola->img.width >= 800) && (bola->x <= 0)) { // ou qualquer outra barreira
        bola->x = direction == 1 ? 800 - bola->img.width : bola->img.width;
        bounce_offset = BOUNCE_OFFSET;
        bola->xspeed = -bola->xspeed * BOUNCE_SPEED_REDUCTION_FACTOR;
        time_passed_x = -1;
      }

      change_y(bola);

      time_passed_y++;
      time_passed_x++;
    }
    else {
      *ball_state = direction == 1 ? AFTER_JUMP_RIGHT : AFTER_JUMP_LEFT;
    }
  }
}

void move_ball(Ball *bola, BallState *ball_state, BallState *ball_state_temporary) {
  switch (*ball_state) {
    case START_JUMP_LEFT:
      initialize_ball_values(bola);
      *ball_state = BEFORE_JUMP_LEFT;
      break;
    case BEFORE_JUMP_LEFT:
      prepare_for_jump(bola);
      *ball_state = bola->yspeed != 0 ? JUMP_LEFT : JUMP_END;
      break;
    case JUMP_LEFT:
      handle_jump(bola, ball_state, -1);
      break;
    case AFTER_JUMP_LEFT:
      update_ball_position_after_jump(bola);
      *ball_state = BEFORE_JUMP_LEFT;
      break;
    case JUMP_END:
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

    case START_JUMP_RIGHT:
      initialize_ball_values(bola);
      *ball_state = BEFORE_JUMP_RIGHT;
      break;
    case BEFORE_JUMP_RIGHT:
      prepare_for_jump(bola);
      *ball_state = bola->yspeed != 0 ? JUMP_RIGHT : JUMP_END;
      break;
    case JUMP_RIGHT:
      handle_jump(bola, ball_state, 1);
      break;
    case AFTER_JUMP_RIGHT:
      update_ball_position_after_jump(bola);
      *ball_state = BEFORE_JUMP_RIGHT;
      break;

    case MOVE_LEFT_START:
      initialize_ball_values(bola);
      *ball_state = MOVE_LEFT;
      break;
    case MOVE_RIGHT_START:
      initialize_ball_values(bola);
      *ball_state = MOVE_RIGHT;
      break;

    case MOVE_LEFT:
      if (get_timer_intCounter() % 2 == 0) {
        bola->x -= bola_xspeed;
      }
      break;

    case MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        bola->x += bola_xspeed;
        printf("bola->x: %d\n", bola->x);
      }
      break;
    case AFTER_MOVE:
      if (bola->xspeed != 0) {
        if (get_timer_intCounter() % 30 == 0) {
          bola->xspeed = bola->xspeed * SPEED_REDUCTION_FACTOR;
        }
      } else {
        *ball_state = JUMP_END;
      }
    default:
      break;
  }
}

int check_ball_goal_height(Ball *bl, Goal *gl) {
  return (bl->y > gl->y) && (bl->y <= gl->img.height + gl->y);
}

void verify_goal(Ball *bl, Goal *gl, Scoreboard *sc, BallState *ball_state) {
  if (gl->orientation) { // right goal
    if ((bl->x >= gl->x) && bl->x <= gl->x + gl->img.width) {
      if (!bl->stop) {
        add_points_1(sc);
        bl->stop = true;
      }
    }
  }
  else { // left goal
    if ((bl->x >= gl->x) && bl->x + bl->img.width <= gl->x + gl->img.width) {
      if (!bl->stop) {
        add_points_2(sc);
        bl->stop = true;
      }
    }
  } 
}

int ball_goal_collision(Ball *bl, Goal *gl, Scoreboard *sc, BallState *ball_state) {
  if (bl == NULL || gl == NULL) return 1;
  
  if (check_ball_goal_height(bl,gl)) {// inside the goal
    verify_goal(bl, gl, sc, ball_state);
    return 0;
  }
  else {
    return 0; // No collision between ball and goal
  }
}

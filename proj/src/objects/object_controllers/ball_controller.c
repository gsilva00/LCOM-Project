#include "ball_controller.h"

uint8_t time_passed_y = 0;
uint8_t time_passed_x = 0;

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

bool check_kicking_player1(Ball *bola, Player *pl) {
  if ((bola->x <= pl->x + pl->width + 10) && (bola->x > pl->x + pl->width)){
    if ((bola->y >= pl->y + pl->height/4) && (bola->y + 5 <= pl->y + pl->height)){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}

bool check_kicking_player2(Ball *bola, Player *pl) {
  if ((bola->x + bola->width >= pl->x - 10) && (bola->x + bola->width < pl->x)){
    if ((bola->y >= pl->y + pl->height/4) && (bola->y + 5 >= pl->y + pl->height)){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}

bool check_border(Ball *bola, Player *pl) {
  if ((bola->x <= pl->x + pl->width + 5) && (bola->x + bola->width + 5 >= pl->x)) {
    return (bola->y <= pl->y + pl->height + 5) && (bola->y + bola->height + 5 >= pl->y);
  }
  else {
    return false;
  }
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

      if ((bola->x + bola->img.width >= BARRIER_START) && (bola->x <= BARRIER_END)) { // ou qualquer outra barreira
        bola->x = direction == 1 ? BARRIER_START + bola->img.width : BARRIER_END + bola->img.width;
        bounce_offset = BOUNCE_OFFSET;
        bola->xspeed = -bola->xspeed * BOUNCE_SPEED_REDUCTION_FACTOR;
        time_passed_x = -1;
      }

      if ((bola->x + bola->img.width >= BARRIER_START_1) && (bola->x <= BARRIER_END_1)) { // ou qualquer outra barreira
        bola->x = direction == 1 ? BARRIER_START_1 - bola->img.width : BARRIER_END_1 - bola->img.width;
        bounce_offset = BOUNCE_OFFSET;
        bola->xspeed = -bola->xspeed * BOUNCE_SPEED_REDUCTION_FACTOR;
        time_passed_x = -1;
      }

      change_y(bola);


      /*if( ball_player_collision(bola, player1)){
        *ball_state = STATE_JUMP_END;
        printf("Colisao\n");
      }*/

      //draw_xpm(bola->x, bola->y, bola->img);

      time_passed_y++;
      time_passed_x++;
    }
    else {
      *ball_state = direction == 1 ? AFTER_JUMP_RIGHT : AFTER_JUMP_LEFT;
    }
  }
}

void move_ball(Ball *bola, BallState *ball_state, BallState *ball_state_temporary, Player *player1) {
  switch (*ball_state) {
    case START_JUMP_LEFT:
      initialize_ball_values(bola);
      *ball_state = BEFORE_JUMP_LEFT;
      break;
    case BEFORE_JUMP_LEFT:
      prepare_for_jump(bola);
      *ball_state = bola->yspeed > 0 ? JUMP_LEFT : JUMP_END;
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
        //draw_xpm(bola->x, bola->y, bola->img);
      }
      break;

    case MOVE_RIGHT:
      if (get_timer_intCounter() % 2 == 0) {
        bola->x += bola_xspeed;
        //draw_xpm(bola->x, bola->y, bola->img);
      }
      break;
    case AFTER_MOVE:
      if (bola->xspeed != 0){
        if (get_timer_intCounter() % 30 == 0) {
          bola->xspeed = bola->xspeed * SPEED_REDUCTION_FACTOR;
        }
      }else{
        *ball_state = JUMP_END;
      }
    default:
      break;
  }
}

int check_ball_goal_height(Ball *bl, Goal *gl) {
  return (bl->y > goal_get_Y(gl)) && (bl->y <= goal_get_height(gl) + goal_get_Y(gl));
}

void verify_goal(Ball *bl, Goal *gl, Scoreboard *sc, BallState *ball_state) {
  if (goal_get_orientation(gl) == 1) {
    if ((bl->x >= goal_get_X(gl)) && bl->x <= goal_get_X(gl) + goal_get_width(gl)) {
      if (!bl->stop) {
        add_points_1(sc);
        bl->stop = true;
      }
    }
    else {
      //printf("not goal"); //not in the goal
    }
  }else if (goal_get_orientation(gl) == 0) {
    if ((bl->x >= goal_get_X(gl)) && bl->x + bl->img.width <= goal_get_X(gl) + goal_get_width(gl)) {
      if (!bl->stop) {
        add_points_2(sc);
        bl->stop = true;
      }
    }
  } 
}

int ball_goal_collision(Ball *bl, Goal *gl, Scoreboard *sc, BallState *ball_state) {
  if (bl == NULL || gl == NULL) return 1; 
  
  if (goal_get_orientation(gl) == 1) { //on the left side of the pitch
    if (check_ball_goal_height(bl,gl)) {//below the
      verify_goal(bl,gl, sc, ball_state);
      return 0;
    }
    else if (bl->y == goal_get_Y(gl)) {
      /*if (bl->x >= goal_get_X(gl) && bl->x < goal_get_X(gl) + goal_get_width(gl)) {
        bl->xspeed *= 9/10;
        bl->yspeed *= -9/10;
        return 0;
      }else if (bl->x == goal_get_X(gl) + goal_get_width(gl)) {
        bl->yspeed *= -9/10;
        if (bl->xspeed < 0) {
          bl->xspeed *= -9/10;
        }else {
          bl->xspeed *= 9/10;
        }
        return 0;
      }else {
        return 0; //not in the goal
      }*/
    }
    else {
      return 0; //No collision between ball and goal
    }
  }
  else if (goal_get_orientation(gl) == 0) {
    if (check_ball_goal_height(bl,gl)) {
      verify_goal(bl,gl,sc, ball_state);
      return 0;
    }
    else if (bl->y == goal_get_Y(gl)) {
      /*if (bl->x > goal_get_X(gl) && bl->x <= goal_get_X(gl) + goal_get_width(gl)) {
        bl->xspeed *= 9/10;
        bl->yspeed *= -9/10;
        return 0;
      }else if (bl->x == goal_get_X(gl)) {
        bl->yspeed *= -9/10;
        if (bl->xspeed < 0) {
          bl->xspeed *= -9/10;
        }else {
          bl->xspeed *= 9/10;
        }
        return 0;
      }else {
        return 0; //not in the goal
      }*/
    }
    else {
      return 0; //No collision between ball and goal
    }
  }
  else {
    return 1; //Direction error
  }

  return 0;
}

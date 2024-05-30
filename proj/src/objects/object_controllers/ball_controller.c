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

void check_border(ball *bola) {
  if (bola->x >= 800) {
    bola->x = 800;   //-=
  }
  if (bola->x <= 0) {
    bola->x = 0;  // += 800
  }
}

void change_y(ball *bola) {
  bola->y = bola_y - ((bola->yspeed * time_passed_y) - 5 * (time_passed_y * time_passed_y));

  if (bola->y >= bola_y_original) {
    bola->y = bola_y_original;
    chuta = false;
  }
}

void handle_jump(ball *bola, BallState *ball_state, int direction, player *player1) {
  if (get_timer_intCounter() % 2 == 0) {
    if (chuta) {
      bola->x = bola_x + direction * (bola->xspeed * time_passed_x) + bounce_offset;

      check_border(bola);

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
      *ball_state = direction == 1 ? STATE_AFTER_JUMP_RIGHT : STATE_AFTER_JUMP_LEFT;
    }
  }
}

void(move_ball)(ball *bola, BallState *ball_state, BallState *ball_state_temporary, player *player1) {
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
      handle_jump(bola, ball_state, -1, player1);
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
      handle_jump(bola, ball_state, 1, player1);
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
      if (bola->xspeed != 0) {
        if (get_timer_intCounter() % 2 == 0) {
          bola->x -= bola->xspeed;
          check_border(bola);          
        }
        if (get_timer_intCounter() % 30 == 0) {
          bola->xspeed = bola->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", bola->xspeed);
        }
      }
      else {
        *ball_state = STATE_JUMP_END;
      }
      break;

    case STATE_MOVE_RIGHT:
      if (bola->xspeed != 0) {
        if (get_timer_intCounter() % 2 == 0) {
          bola->x += bola->xspeed;
          check_border(bola);
        }
        if (get_timer_intCounter() % 30 == 0) {
          bola->xspeed = bola->xspeed * SPEED_REDUCTION_FACTOR;
          printf("%d", bola->xspeed);
        }
      }
      else {
        *ball_state = STATE_JUMP_END;
      }
      break;
    default:
      break;
  }
}

int check_ball_goal_height(ball *bl, goal *gl){
  return (bl->y > goal_get_Y(gl)) && (bl->y <= goal_get_height(gl) + goal_get_Y(gl));
}

void verify_goal(ball *bl, goal *gl, scoreboard *sc, BallState *ball_state){
  if(goal_get_orientation(gl) == 1){
    if((bl->x >= goal_get_X(gl)) && bl->x <= goal_get_X(gl) + goal_get_width(gl)){
      if (!bl->stop)
      {
        add_points_1(sc);
        bl->stop = true;
      }
    }else{
      //printf("not goal"); //not in the goal
    }
  }else if(goal_get_orientation(gl) == 0){
    if((bl->x >= goal_get_X(gl)) && bl->x + bl->img.width <= goal_get_X(gl) + goal_get_width(gl)){
      if (!bl->stop)
      {
        add_points_2(sc);
        bl->stop = true;
      }
    }
  } 
}

int ball_goal_collision(ball *bl, goal *gl, scoreboard *sc, BallState *ball_state){
  if(bl == NULL || gl == NULL){
    return 1;
  }else{
    if(goal_get_orientation(gl) == 1){ //on the left side of the pitch
      if(check_ball_goal_height(bl,gl)){//below the
        verify_goal(bl,gl, sc, ball_state);
        return 0;
      }else if(bl->y == goal_get_Y(gl)){
        /*if(bl->x >= goal_get_X(gl) && bl->x < goal_get_X(gl) + goal_get_width(gl)){
          bl->xspeed *= 9/10;
          bl->yspeed *= -9/10;
          return 0;
        }else if(bl->x == goal_get_X(gl) + goal_get_width(gl)){
          bl->yspeed *= -9/10;
          if(bl->xspeed < 0){
            bl->xspeed *= -9/10;
          }else{
            bl->xspeed *= 9/10;
          }
          return 0;
        }else{
          return 0; //not in the goal
        }*/
      }else{
        return 0; //No collision between ball and goal
      }
    }else if(goal_get_orientation(gl) == 0){
      if(check_ball_goal_height(bl,gl)){
        verify_goal(bl,gl,sc, ball_state);
        return 0;
      }else if(bl->y == goal_get_Y(gl)){
        /*if(bl->x > goal_get_X(gl) && bl->x <= goal_get_X(gl) + goal_get_width(gl)){
          bl->xspeed *= 9/10;
          bl->yspeed *= -9/10;
          return 0;
        }else if(bl->x == goal_get_X(gl)){
          bl->yspeed *= -9/10;
          if(bl->xspeed < 0){
            bl->xspeed *= -9/10;
          }else{
            bl->xspeed *= 9/10;
          }
          return 0;
        }else{
          return 0; //not in the goal
        }*/
      }else{
        return 0; //No collision between ball and goal
      }
    }else{
      return 1; //Direction error
    }
  }
  return 0;
}

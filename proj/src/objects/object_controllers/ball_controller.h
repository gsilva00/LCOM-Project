#ifndef BALL_CONTROLLER_H
#define BALL_CONTROLLER_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 200
#define BARRIER_START 0
#define BARRIER_END 20
#define BARRIER_START_1 780
#define BARRIER_END_1 800
#define SPEED_REDUCTION_FACTOR 0.65
#define BOUNCE_SPEED_REDUCTION_FACTOR 0.85
#define BOUNCE_OFFSET 50

#include <lcom/lcf.h>
#include <stdint.h>
#include <objects/ball.h>
#include <devices/timer/aux_timer.h>
#include <objects/scoreboard.h>
#include <objects/goal.h>

  int bola_y_original;
  int bola_y;
  int bola_x;
  int bola_yspeed;
  int bola_xspeed;
  int bounce_offset;
  bool chuta;

  typedef enum {
    STATE_NONE,
    STATE_START_JUMP_LEFT,
    STATE_BEFORE_JUMP_LEFT,
    STATE_JUMP_LEFT,
    STATE_AFTER_JUMP_LEFT,
    STATE_JUMP_END,
    STATE_START_JUMP_RIGHT,
    STATE_BEFORE_JUMP_RIGHT,
    STATE_JUMP_RIGHT,
    STATE_AFTER_JUMP_RIGHT,

    STATE_MOVE_LEFT_START,
    STATE_MOVE_RIGHT_START,
    STATE_MOVE_LEFT,
    STATE_MOVE_RIGHT,
    STATE_AFTER_MOVE,
  } BallState;

void(move_ball)(ball *bola, BallState *ball_state, BallState * ball_state_temporary, player *player1);
bool check_border(ball *bola, player *pl);

int ball_goal_collision(ball *bl, goal *gl, scoreboard *sc,BallState * ball_state);

bool check_kicking_player1(ball *bola, player *pl);

bool check_kicking_player2(ball *bola, player *pl);

#endif

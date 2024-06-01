#ifndef BALL_CONTROLLER_H
#define BALL_CONTROLLER_H

#include <lcom/lcf.h>

#include <stdint.h>

#include <devices/timer/aux_timer.h>

#include <objects/ball.h>
#include <objects/scoreboard.h>
#include <objects/goal.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 200
#define BARRIER_START 0
#define BARRIER_END 20
#define BARRIER_START_1 780
#define BARRIER_END_1 800
#define SPEED_REDUCTION_FACTOR 0.65
#define BOUNCE_SPEED_REDUCTION_FACTOR 0.85
#define BOUNCE_OFFSET 50

int bola_y_original;
int bola_y;
int bola_x;
int bola_yspeed;
int bola_xspeed;
int bounce_offset;
bool chuta;

typedef enum {
  STATE_NONE,
  START_JUMP_LEFT,
  BEFORE_JUMP_LEFT,
  JUMP_LEFT,
  AFTER_JUMP_LEFT,
  JUMP_END,
  START_JUMP_RIGHT,
  BEFORE_JUMP_RIGHT,
  JUMP_RIGHT,
  AFTER_JUMP_RIGHT,

  MOVE_LEFT_START,
  MOVE_RIGHT_START,
  MOVE_LEFT,
  MOVE_RIGHT,
  AFTER_MOVE,
} BallState;

void move_ball(Ball *bola, BallState *ball_state, BallState * ball_state_temporary, Player *player1);
bool check_border(Ball *bola, Player *pl);

int ball_goal_collision(Ball *bl, Goal *gl, Scoreboard *sc,BallState * ball_state);

bool check_kicking_player1(Ball *bola, Player *pl);

bool check_kicking_player2(Ball *bola, Player *pl);

#endif

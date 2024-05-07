#ifndef BALL_H
#define BALL_H

#include <lcom/lcf.h>
#include "ball.h"
#include "player.h"
#include "goal.h"

//struct Ball;

struct Ball{
  int x, y;
  int width, height;
  int xspeed, yspeed;
  uint32_t time_in_mov;
  uint8_t *map;
  bool stop;
  xpm_image_t img;
};

typedef struct Ball ball;

ball *create_ball(xpm_map_t pic, int x, int y, int xspeed, int yspeed, uint32_t time_in_mov);

void destroy_ball(ball *bl);

/*int move_ball(ball *bl);*/

int ball_walls_collision(ball *bl, vbe_mode_info_t vmi_p);

int ball_player_collision(ball *bl, player *pl);

int ball_goal_collision(ball *bl, goal *gl);

int detect_collisions(ball *bl, vbe_mode_info_t vmi_p, player *p1, player *p2, goal *gl1, goal *gl2);

#endif

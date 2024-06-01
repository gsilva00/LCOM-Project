#ifndef BALL_H
#define BALL_H

#include <lcom/lcf.h>

#include "ball.h"
#include "player.h"
#include "goal.h"


typedef struct {
  uint16_t x, y;
  int width, height;
  int xspeed, yspeed;
  uint32_t time_in_mov;
  uint8_t *map;
  bool stop;
  xpm_image_t img;
} Ball;


Ball *create_ball(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, uint32_t time_in_mov);

void destroy_ball(Ball *bl);

/*int move_ball(ball *bl);*/

int ball_walls_collision(Ball *bl, vbe_mode_info_t vmi_p);

int ball_player_collision(Ball *bl, Player *pl);

int detect_collisions(Ball *bl, vbe_mode_info_t vmi_p, Player *p1, Player *p2, Goal *gl1, Goal *gl2);

void move_ball_to_center(Ball *bl);

#endif

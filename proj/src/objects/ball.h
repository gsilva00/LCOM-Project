#ifndef BALL_H
#define BALL_H

//struct Ball;

struct Ball{
  int x, y;
  int width, height;
  int xspeed, yspeed;
  uint32_t time_in_mov;
  uint8_t *map;
  xpm_image_t img;
};

typedef struct Ball ball;

ball *create_ball(xpm_map_t pic, int x, int y, int xspeed, int yspeed, uint32_t time_in_mov);

void destroy_ball(ball *bl);

int move_ball(ball *bl);



#endif

#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <lcom/lcf.h>

struct Scoreboard{
  uint16_t x, y;
  uint8_t points1;
  uint8_t points2;
  xpm_image_t points1_img;
  uint8_t *map;
  xpm_image_t img;
};

typedef struct Scoreboard scoreboard;

scoreboard *create_scoreboard(xpm_map_t pic);

void destroy_scoreboard(scoreboard *sc);

void add_points_1(scoreboard *sc);

void add_points_2(scoreboard *sc);

#endif

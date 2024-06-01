#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <lcom/lcf.h>

typedef struct {
  uint16_t x, y;
  uint8_t points1;
  uint8_t points2;
  xpm_image_t points1_img;
  uint8_t *map;
  xpm_image_t img;
} Scoreboard;

Scoreboard *create_scoreboard(xpm_map_t pic);

void destroy_scoreboard(Scoreboard *sc);

void add_points_1(Scoreboard *sc);

void add_points_2(Scoreboard *sc);

#endif

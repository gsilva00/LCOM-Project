#ifndef TIMEBOARD_H
#define TIMEBOARD_H

#include <lcom/lcf.h>

#include "../devices/timer/aux_timer.h"


typedef struct {
  uint16_t x, y;
  int time;
  xpm_image_t points1_img;
  uint8_t *map;
  xpm_image_t img;
} Timeboard;

Timeboard *create_timeboard(xpm_map_t pic);

void destroy_timeboard(Timeboard *sc);

void decrease_time(Timeboard *sc);

#endif

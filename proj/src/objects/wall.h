#ifndef WALL_H
#define WALL_H

#include <lcom/lcf.h>


typedef struct {
  int width, height;
  uint8_t *map;
  xpm_image_t img;
} Wall;


Wall *create_wall(xpm_map_t pic);

void destroy_wall(Wall *wl);


#endif

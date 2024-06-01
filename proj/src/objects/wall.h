#ifndef WALL_H
#define WALL_H

#include <lcom/lcf.h>



struct Wall{
  int width, height;
  uint8_t *map;
  xpm_image_t img;
};
typedef struct Wall wall;


wall *create_wall(xpm_map_t pic);

#endif

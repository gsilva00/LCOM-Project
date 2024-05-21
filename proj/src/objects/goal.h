#ifndef GOAL_H
#define GOAL_H

#include <lcom/lcf.h>

struct Goal{
  int width, height;
  uint8_t *map;
  xpm_image_t img;
};

typedef struct Goal goal;

goal *create_goal(xpm_map_t pic);

//void destroy_goal(goal *pl);

//int move_goal(goal *pl);

//int goal_get_X(goal *gl);

//int goal_get_Y(goal *gl);

//int goal_get_width(goal *gl);

//int goal_get_height(goal *gl);

//int goal_get_orientation(goal *gl);

#endif

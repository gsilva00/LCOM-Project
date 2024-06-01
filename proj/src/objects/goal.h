#ifndef GOAL_H
#define GOAL_H

#include <lcom/lcf.h>


typedef struct {
  int x,y;
  bool orientation;
  uint8_t *map;
  xpm_image_t img;
} Goal;

Goal *create_goal(xpm_map_t pic, bool orientation);

void destroy_goal(Goal *pl);

int move_goal(Goal *pl);

int goal_get_X(Goal *gl);

int goal_get_Y(Goal *gl);

int goal_get_width(Goal *gl);

int goal_get_height(Goal *gl);

int goal_get_orientation(Goal *gl);

#endif

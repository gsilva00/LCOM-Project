#ifndef GOAL_H
#define GOAL_H

#include <lcom/lcf.h>

struct Goal;
typedef struct Goal goal;

goal *create_goal(char *pic[], int x, int y);

void destroy_goal(goal *pl);

int move_goal(goal *pl);

#endif
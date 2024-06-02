#ifndef GOAL_H
#define GOAL_H

#include <lcom/lcf.h>

/**
 * @struct Goal
 * @brief Implementation of the Goal as an object usable in game
 * 
 * @var Goal::x
 * Value of the Goal's position in the x axis
 * 
 * @var Goal::y
 * Value of the Goal's position in the y axis
 * 
 * @var Goal::orientation
 * Value of the Goal's orientation
 * True for right side goal and false for left side goal
 * 
 * @var Goal::map
 * Pointer to the Goal's xpm map
 * 
 * @var Goal::img
 * Value of the Goal's img
 */
typedef struct {
  int x,y;
  bool orientation;
  uint8_t *map;
  xpm_image_t img;
} Goal;

/** 
 * @brief Constructor of the Goal "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Goal instance, effectively initializing the object
 *
 * @param pic XPM map used to make the goal's image
 * @param orientation Value of the orientation of the goal
 * @return Pointer to the Goal struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Goal *create_goal(xpm_map_t pic, bool orientation);

/** 
 * @brief Destructor of the Goal "class"
 * @details Frees the memory allocated for the Goal instance and its buffer
 *
 * @param gl Pointer to the Goal struct to be deallocated. If gl is NULL, no operation is performed
 */
void destroy_goal(Goal *gl);

#endif

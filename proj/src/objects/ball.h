#ifndef BALL_H
#define BALL_H

#include <lcom/lcf.h>

#include "ball.h"
#include "player.h"
#include "goal.h"


/**
 * @struct Ball
 * @brief Implementation of the Ball as an object usable in game
 * 
 * @var Ball::x
 * Value of the Ball's position in the x axis
 * 
 * @var Ball::y
 * Value of the Ball's position in the y axis
 * 
 * @var Ball::width
 * Value of the Ball's image width
 * 
 * @var Ball::height
 * Value of the Ball's image height
 * 
 * @var Ball::xspeed
 * Value of the Ball's speed in the x axis
 * 
 * @var Ball::yspeed
 * Value of the Ball's speed in the y axis
 * 
 * @var Ball::map
 * Pointer to the Ball's xpm map
 * 
 * @var Ball::stop
 * Variable that says if the Ball has stopped
 * 
 * @var Ball::img
 * Value of the Ball's img
 */
typedef struct {
  uint16_t x, y;
  int width, height;
  int xspeed, yspeed;
  uint8_t *map;
  bool stop;
  xpm_image_t img;
} Ball;

/** 
 * @brief Constructor of the ball "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Ball instance, effectively initializing the object
 *
 * @param pic XPM map used to make the ball's image
 * @param x Value of the ball's position in the x axis
 * @param y Value of the ball's position in the y axis
 * @param width Value of the ball's image width
 * @param height Value of the ball's image height
 * @param xspeed Value of the ball's speed in the x axis
 * @param yspeed Value of the ball's speed in the y axis
 * @return Pointer to the Ball struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Ball *create_ball(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed);

/** 
 * @brief Destructor of the ball "class"
 * @details Frees the memory allocated for the Ball instance and its buffer
 *
 * @param bl Pointer to the Ball struct to be deallocated. If bl is NULL, no operation is performed
 */
void destroy_ball(Ball *bl);

/** 
 * @brief Function that moves the ball to the center
 * @details Moves the ball to the center of the field. It is used whenever the game is reset, either by beginning a new game or when someone scores a goal
 *
 * @param bl Pointer to the Ball struct to be reallocated. If bl is NULL, no operation is performed
 */
void move_ball_to_center(Ball *bl);

#endif

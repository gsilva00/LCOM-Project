#ifndef TIMEBOARD_H
#define TIMEBOARD_H

#include <lcom/lcf.h>

#include "../devices/timer/aux_timer.h"

/**
 * @struct Timeboard
 * @brief Implementation of the Timeboard as an object usable in game
 * 
 * @var Timeboard::x
 * Value of the Timeboard's position in the x axis
 * 
 * @var Timeboard::time
 * Value of the Timeboard's time
 * 
 * @var Timeboard::points1_img
 * Value of the image of the first part of the timeboard
 * 
 * @var Timeboard::map
 * XPM map of the Timeboard
 * 
 * @var Timeboard::img
 * Value of the Timeboard's image
 */
typedef struct {
  uint16_t x, y;
  int time;
  xpm_image_t points1_img;
  uint8_t *map;
  xpm_image_t img;
} Timeboard;

/** 
 * @brief Constructor of the Timeboard "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Timeboard instance, effectively initializing the object
 *
 * @param pic XPM map used to make the timeboard's image
 * @return Pointer to the Timeboard struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Timeboard *create_timeboard(xpm_map_t pic);

/** 
 * @brief Destructor of the Timeboard "class"
 * @details Frees the memory allocated for the Timeboard instance and its buffer
 *
 * @param tb Pointer to the Timeboard struct to be deallocated. If tb is NULL, no operation is performed
 */
void destroy_timeboard(Timeboard *tb);

/** 
 * @brief Function that decreaces the time left
 * @details This function changes the image of the first and second part of the timeboard according to the decreasing remaining time left
 *
 * @param tb Pointer to the Timeboard struct to be deallocated. If tb is NULL, no operation is performed
 */
void decrease_time(Timeboard *tb);

#endif

#ifndef WALL_H
#define WALL_H

#include <lcom/lcf.h>

/**
 * @struct Wall
 * @brief Implementation of the Wall as an object usable in game
 * 
 * @var Wall::width
 * Value of the Wall's image width
 * 
 * @var Wall::height
 * Value of the Wall's image height
 * 
 * @var Wall::map
 * Pointer to the Wall's xpm map
 * 
 * @var Wall::img
 * Value of the Wall's img
 */
typedef struct {
  int width, height;
  uint8_t *map;
  xpm_image_t img;
} Wall;

/** 
 * @brief Constructor of the Wall "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Wall instance, effectively initializing the object
 *
 * @param pic XPM map used to make the wall's image
 * @return Pointer to the Wall struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Wall *create_wall(xpm_map_t pic);

/** 
 * @brief Destructor of the Wall "class"
 * @details Frees the memory allocated for the wall instance and its buffer
 *
 * @param wl Pointer to the Wall struct to be deallocated. If wl is NULL, no operation is performed
 */
void destroy_wall(Wall *wl);


#endif

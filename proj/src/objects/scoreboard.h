#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <lcom/lcf.h>

/**
 * @struct Scoreboard
 * @brief Implementation of the Scoreboard as an object usable in game
 * 
 * @var Scoreboard::x
 * Value of the Scoreboard's position in the x axis
 * 
 * @var Scoreboard::y
 * Value of the Scoreboard's position in the y axis
 * 
 * @var Scoreboard::points1
 * Value of the first player's score
 * 
 * @var Scoreboard::points2
 * Value of the second player's score
 * 
 * @var Scoreboard::points1_img
 * Value of first player's points image
 * 
 * @var Scoreboard::map
 * XPM map of the scoreboard
 * 
 * @var Scoreboard::img
 * Value of the scoreboard's image
 */
typedef struct {
  uint16_t x, y;
  uint8_t points1;
  uint8_t points2;
  xpm_image_t points1_img;
  uint8_t *map;
  xpm_image_t img;
} Scoreboard;

/** 
 * @brief Constructor of the Scoreboard "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Scoreboard instance, effectively initializing the object
 *
 * @param pic XPM map used to make the scoreboard's image
 * @return Pointer to the Scoreboard struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Scoreboard *create_scoreboard(xpm_map_t pic);

/** 
 * @brief Destructor of the Scoreboard "class"
 * @details Frees the memory allocated for the Scoreboard instance and its buffer
 *
 * @param sc Pointer to the Scoreboard struct to be deallocated. If bl is NULL, no operation is performed
 */
void destroy_scoreboard(Scoreboard *sc);

/** 
 * @brief Function that adds a goal to the first player's score
 * @details This function changes the image of the first part of the score to the next number after a goal
 *
 * @param sc Pointer to the Scoreboard struct to be deallocated. If sc is NULL, no operation is performed
 */
void add_points_1(Scoreboard *sc);

/** 
 * @brief Function that adds a goal to the second player's score
 * @details This function changes the image of the second part of the score to the next number after a goal
 *
 * @param sc Pointer to the Scoreboard struct to be deallocated. If sc is NULL, no operation is performed
 */
void add_points_2(Scoreboard *sc);

#endif

#ifndef CURSOR_H
#define CURSOR_H

#include <lcom/lcf.h>

/**
 * @struct Cursor
 * @brief Implementation of the Cursor as an object usable in game
 * 
 * @var Cursor::x
 * Value of the Cursor's position in the x axis
 * 
 * @var Cursor::y
 * Value of the Cursor's position in the y axis
 * 
 * @var Cursor::img
 * Value of the Cursor's img
 */
typedef struct {
  uint16_t x, y;
  xpm_image_t img;
} Cursor;

/** 
 * @brief Constructor of the Cursor "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Cursor instance, effectively initializing the object
 *
 * @param pic XPM map used to make the cursor's image
 * @return Pointer to the Cursor struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Cursor *create_cursor(xpm_map_t pic);

/** 
 * @brief Destructor of the Cursor "class"
 * @details Frees the memory allocated for the Cursor instance and its buffer
 *
 * @param cr Pointer to the Cursor struct to be deallocated. If cr is NULL, no operation is performed
 */
void destroy_cursor(Cursor *cr);

/** 
 * @brief Function that sets the new coordinates of the cursor
 * @details This function takes an x, y, negative_x, neagtive_y, and alters the cursor's coordinates given the mouse movement
 *
 * @param cr Pointer to the Cursor struct to be reallocated. If cr is NULL, no operation is performed
 * @param x Value of the cursor's position in the x axis
 * @param y Value of the cursor's position in the y axis
 * @param negative_x Variable that signals if the x is negative
 * @param negative_y Variable that signals if the y is negative
 */
void set_coordinates(Cursor *cr, uint16_t x, uint16_t y, bool negative_x , bool negative_y);

#endif

#ifndef BUTTON_H
#define BUTTON_H

#include <lcom/lcf.h>


/**
 * @struct Button
 * @brief Implementation of the Button as an object usable in game
 * 
 * @var Button::x
 * Value of the Button's position in the x axis
 * 
 * @var Button::y
 * Value of the Button's position in the y axis
 * 
 * @var Button::width
 * Value of the Button's image width
 * 
 * @var Button::height
 * Value of the Button's image height
 * 
 * @var Button::map
 * Pointer to the Button's xpm map
 * 
 * @var Button::selected
 * Variable that says if the Button was selected
 * 
 * @var Button::img
 * Value of the Button's img
 */
typedef struct {
  int x, y;
  int width, height;
  uint8_t *map;
  bool selected;
  xpm_image_t img;
} Button;

/** 
 * @brief Constructor of the Button "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Button instance, effectively initializing the object
 *
 * @param pic XPM map used to make the button's image
 * @param x Value of the button's position in the x axis
 * @param y Value of the button's position in the y axis
 * @param selected Variable that says if the Button was selected
 * @return Pointer to the Button struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Button *create_button(xpm_map_t pic, int x, int y, bool selected);

/** 
 * @brief Destructor of the Button "class"
 * @details Frees the memory allocated for the Button instance and its buffer
 *
 * @param bt Pointer to the Button struct to be deallocated. If bt is NULL, no operation is performed
 */
void destroy_button(Button *bt);

#endif

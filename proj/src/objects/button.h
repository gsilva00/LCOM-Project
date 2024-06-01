#ifndef BUTTON_H
#define BUTTON_H

#include <lcom/lcf.h>


typedef struct {
  int x, y;
  int width, height;
  uint8_t *map;
  bool selected;
  xpm_image_t img;
} Button;

Button *create_button(xpm_map_t pic, int x, int y, bool selected);
void destroy_button(Button *bl);
void set_selected(Button *bt, bool selected);
bool get_selected(Button *bt);
void set_image(Button *bt, xpm_map_t map);

#endif

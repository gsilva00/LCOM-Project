#ifndef BUTTON_H
#define BUTTON_H

#include <lcom/lcf.h>


struct Button{
  int x, y;
  int width, height;
  uint8_t *map;
  bool selected;
  xpm_image_t img;
};
typedef struct Button button;

button *create_button(xpm_map_t pic, int x, int y, bool selected);
void destroy_button(button *bl);
void set_selected(button *bt, bool selected);
bool get_selected(button *bt);
void set_image(button *bt, xpm_map_t map);

#endif

#ifndef CURSOR_H
#define CURSOR_H

#include <lcom/lcf.h>


typedef struct {
  uint16_t x, y;
  xpm_image_t img;
} Cursor;

Cursor *create_cursor(xpm_map_t pic);

void destroy_cursor(Cursor *bl);

void set_coordinates(Cursor *cr, uint16_t x, uint16_t y, bool negative_x , bool negative_y);

#endif

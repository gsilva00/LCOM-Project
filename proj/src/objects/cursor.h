#ifndef CURSOR_H
#define CURSOR_H

#include <lcom/lcf.h>


struct Cursor{
  uint16_t x, y;
  xpm_image_t img;
};
typedef struct Cursor cursor;

cursor *create_cursor(xpm_map_t pic);

void destroy_cursor(cursor *bl);

void set_coordinates(cursor *cr, uint16_t x, uint16_t y, bool negative_x , bool negative_y);

#endif

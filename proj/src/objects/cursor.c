#include "cursor.h"

cursor *create_cursor(xpm_map_t pic) {
  cursor *cr = (cursor *) malloc ( sizeof(cursor));
  if(cr == NULL){
    return NULL;
  }
  xpm_image_t img;
  uint8_t *map = xpm_load(pic, XPM_8_8_8,&img);
  
  if(map == NULL ) {
    free(cr);
    return NULL;
  }
  cr->img = img;
  cr->x = 800;
  cr->y = 600;
  return cr;
}

void destroy_cursor(cursor *cr) {
  if(cr == NULL)
    return;
  free(cr);
  cr = NULL;
}

void set_coordinates(cursor *cr, uint16_t x, uint16_t y, bool negative_x , bool negative_y) {
  if(cr == NULL)
    return;
  if(negative_x){
     cr->x -= x;
  }else{
  cr->x += x;
  }
  if(cr->y - y > 600){
      cr->y = 0;
    }else{
      cr->y -= y;
    }
  if (cr->x < 0)
  {
    cr->x = 0;
  }
  if (cr->y < 0)
  {
    cr->y = 0;
  }
  if (cr->x > 800)
  {
    cr->x = 800;
  }
  if (cr->y > 600)
  {
    cr->y = 600;
  }
  
  
}

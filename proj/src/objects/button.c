#include "button.h"

button *create_button(xpm_map_t pic, int x, int y, bool selected) {
  //allocate space for the "object"
  button *bl = (button *) malloc ( sizeof(button));
  if(bl == NULL){
    return NULL;
  }
  xpm_image_t img;
  // read the button pixmap
  bl->map = xpm_load(pic, XPM_8_8_8,&img);
  
  if( bl->map == NULL ) {
    free(bl);
    return NULL;
  }
  bl->x = x;
  bl->y = y;
  bl->img = img;
  bl->selected = selected;
  return bl;
}

void destroy_button(button *bl) {
  if(bl == NULL)
    return;
  if(bl->map )
    free(bl->map);
  free(bl);
  bl = NULL;
}

void set_selected(button *bt, bool selected){
  if(bt == NULL){
    return;
  }
  bt->selected = selected;
}

void set_image(button *bt, xpm_map_t map){
  if(bt == NULL){
    return;
  }
  bt->map = xpm_load(map, XPM_8_8_8, &bt->img);
}

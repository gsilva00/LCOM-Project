#include "button.h"


Button *create_button(xpm_map_t pic, int x, int y, bool selected) {
  //allocate space for the "object"
  Button *bt = (Button *) malloc (sizeof(Button));
  if (bt == NULL) return NULL;

  xpm_image_t img;
  // read the button pixmap
  bt->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if ( bt->map == NULL ) {
    free(bt);
    return NULL;
  }

  bt->x = x;
  bt->y = y;
  bt->img = img;
  bt->selected = selected;
  
  return bt;
}

void destroy_button(Button *bt) {
  if (bt == NULL) return;
  if (bt->map) free(bt->map);
  free(bt);
  bt = NULL;
}

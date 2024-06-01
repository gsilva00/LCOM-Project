#include "wall.h"


Wall *create_wall(xpm_map_t pic) {
  Wall *wl = (Wall *) malloc (sizeof(Wall));
  if (wl == NULL) return NULL;

  xpm_image_t img;
  // read the player pixmap
  wl->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if (wl->map == NULL) {
    free(wl);
    return NULL;
  }

  wl->img = img;
  
  return wl;
}

void destroy_wall(Wall *wl) {
  if (wl == NULL) return;
  if (wl->map) free(wl->map);
  free(wl);
  wl = NULL;
}

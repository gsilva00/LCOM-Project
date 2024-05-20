#include "wall.h"

wall *create_wall(xpm_map_t pic) {
  wall *wl = (wall *) malloc ( sizeof(wall));
  if(wl == NULL){
    return NULL;
  }
  xpm_image_t img;
  // read the player pixmap
  wl->map = xpm_load(pic, XPM_8_8_8,&img);
  
  if( wl->map == NULL ) {
    free(wl);
    return NULL;
  }
  wl->img = img;
  return wl;
}

#include "goal.h"

Goal *create_goal(xpm_map_t pic, bool orientation) {
  //allocate space for the "object"
  Goal *gl = (Goal *) malloc (sizeof(Goal));
  if (gl == NULL) return NULL;

  xpm_image_t img;
  gl->map = xpm_load(pic, XPM_8_8_8, &img);
  
  if ( gl->map == NULL ) {
    free(gl);
    return NULL;
  }

  gl->img = img;
  gl->orientation = orientation;
  
  if (orientation) gl->x = 720; // Right goal
  else gl->x = 0; // Left goal

  gl->y = 314;
  return gl;
}

void destroy_goal(Goal *gl) {
  if (gl == NULL) return;
  if (gl->map) free(gl->map);
  free(gl);
  gl = NULL;
}

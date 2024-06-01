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

 
int goal_get_X(Goal *gl) {
  if (gl == NULL) return -1;

  return gl->x;
}

int goal_get_Y(Goal *gl) {
  if (gl == NULL) return 1;
  
  return gl->y;
}

int goal_get_width(Goal *gl) {
  if (gl == NULL) return -1;

  return gl->img.width  ;
}

int goal_get_height(Goal *gl) {
  if (gl == NULL) return -1;

  return gl->img.height;
}


int goal_get_orientation(Goal *gl) {
  if (gl == NULL) return -1;
  
  return gl->orientation;
}

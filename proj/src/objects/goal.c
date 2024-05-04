#include <lcom/lcf.h>
#include "goal.h"-*

struct Goal{
  int x, y;
  int width, height;
  char *map;
};

goal *create_goal(char *pic[], int x, int y) {
  //allocate space for the "object"
  goal *gl = (goal *) malloc ( sizeof(goal));
  if(gl == NULL)
    return NULL;
  // read the goal pixmap
  gl->map = read_xpm(pic, &(gl->width), &(gl->height));
  if( gl->map == NULL ) {
    free(gl);
    return NULL;
  }
  gl->x = x;
  gl->y = y;
  return gl;
}

void destroy_goal(goal *gl) {
  if( gl == NULL )
    return;
  if( gl->map )
    free(gl->map);
  free(gl);
  gl = NULL;
}
